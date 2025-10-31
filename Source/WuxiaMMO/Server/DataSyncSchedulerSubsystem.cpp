#include "Server/DataSyncSchedulerSubsystem.h"

#include "Dom/JsonObject.h"
#include "Engine/World.h"
#include "HAL/CriticalSection.h"
#include "Logging/LogMacros.h"
#include "Misc/Base64.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/DateTime.h"
#include "Misc/ScopeLock.h"
#include "Security/AESUtil.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Server/PersistenceManagerSubsystem.h"
#include "Server/SnapshotManager.h"
#include "Stats/Stats.h"

namespace
{
    void CopyStringToUtf8Bytes(const FString& InString, TArray<uint8>& OutBytes)
    {
        FTCHARToUTF8 Converter(*InString);
        OutBytes.SetNumUninitialized(Converter.Length());
        if (Converter.Length() > 0)
        {
            FMemory::Memcpy(OutBytes.GetData(), Converter.Get(), Converter.Length());
        }
    }
}

void UDataSyncSchedulerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    GConfig->GetFloat(TEXT("Wuxia.Sync"), TEXT("AutosaveIntervalSeconds"), AutosaveInterval, GGameIni);
    GConfig->GetFloat(TEXT("Wuxia.Sync"), TEXT("SnapshotIntervalSeconds"), SnapshotInterval, GGameIni);
    GConfig->GetString(TEXT("Wuxia.Sync"), TEXT("SnapshotDirectory"), SnapshotDir, GGameIni);
    GConfig->GetInt(TEXT("Wuxia.Sync"), TEXT("MaxSnapshots"), SnapshotKeep, GGameIni);

    USnapshotManager::Initialize(SnapshotDir, SnapshotKeep);

    if (UWorld* World = GetWorld())
    {
        Persistence = World->GetSubsystem<UPersistenceManagerSubsystem>();
        if (Persistence.IsValid())
        {
            Persistence->InitFromConfig();
        }
    }
}

void UDataSyncSchedulerSubsystem::Deinitialize()
{
    FlushNow();
    Super::Deinitialize();
}

bool UDataSyncSchedulerSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
    return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UDataSyncSchedulerSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UDataSyncSchedulerSubsystem, STATGROUP_Tickables);
}

void UDataSyncSchedulerSubsystem::Tick(float DeltaTime)
{
    if (BackoffTimer > 0.f)
    {
        BackoffTimer -= DeltaTime;
        return;
    }

    AutosaveTimer += DeltaTime;
    SnapshotTimer += DeltaTime;

    if (AutosaveInterval > 0.f && AutosaveTimer >= AutosaveInterval)
    {
        AutosaveTimer = 0.f;
        FlushNow();
    }

    if (SnapshotInterval > 0.f && SnapshotTimer >= SnapshotInterval)
    {
        SnapshotTimer = 0.f;
        WriteSnapshotNow();
    }
}

void UDataSyncSchedulerSubsystem::EnqueueJob(const FSyncJob& Job)
{
    FScopeLock Lock(&QueueMutex);
    Queue.Add(Job);
}

bool UDataSyncSchedulerSubsystem::DrainQueueToInFlight()
{
    FScopeLock Lock(&QueueMutex);
    if (Queue.Num() == 0)
    {
        return false;
    }

    InFlight = MoveTemp(Queue);
    Queue.Reset();
    return InFlight.Num() > 0;
}

bool UDataSyncSchedulerSubsystem::BatchPersist(TArray<FSyncJob>& Batch)
{
    if (!Persistence.IsValid())
    {
        return false;
    }

    bool bNeedsKey = false;
    for (const FSyncJob& Job : Batch)
    {
        if (Job.Sensitivity == ESyncSensitivity::Sensitive)
        {
            bNeedsKey = true;
            break;
        }
    }

    TArray<uint8> Key;
    if (bNeedsKey && !AESUtil::LoadKeyFromEnv(Key))
    {
        UE_LOG(LogTemp, Error, TEXT("DataSyncSchedulerSubsystem: Missing AES key environment variable WUXIA_AES_KEY_32_BASE64."));
        return false;
    }

    auto BuildEncryptedBatch = [&](ESyncDomain DomainFilter, TArray<FSyncJob>& OutJobs)
    {
        for (const FSyncJob& Job : Batch)
        {
            if (Job.Domain != DomainFilter)
            {
                continue;
            }

            FSyncJob Processed = Job;
            if (Processed.Sensitivity == ESyncSensitivity::Sensitive)
            {
                TArray<uint8> PlainBytes;
                CopyStringToUtf8Bytes(Processed.JsonPayload, PlainBytes);

                const FAESGCMEncryptResult Enc = AESUtil::Encrypt(PlainBytes, Key);
                if (!Enc.bOk)
                {
                    UE_LOG(LogTemp, Error, TEXT("DataSyncSchedulerSubsystem: Failed to encrypt sensitive payload for topic %s."), *Processed.Topic);
                    OutJobs.Reset();
                    return false;
                }

                TArray<uint8> Combined;
                Combined.Reserve(Enc.Nonce.Num() + Enc.Tag.Num() + Enc.Ciphertext.Num());
                Combined.Append(Enc.Nonce);
                Combined.Append(Enc.Tag);
                Combined.Append(Enc.Ciphertext);

                Processed.JsonPayload = FBase64::Encode(Combined);
            }

            OutJobs.Add(MoveTemp(Processed));
        }

        return true;
    };

    TArray<FSyncJob> PlayerJobs;
    TArray<FSyncJob> WorldJobs;
    TArray<FSyncJob> ServerJobs;

    if (!BuildEncryptedBatch(ESyncDomain::Player, PlayerJobs))
    {
        return false;
    }
    if (!BuildEncryptedBatch(ESyncDomain::World, WorldJobs))
    {
        return false;
    }
    if (!BuildEncryptedBatch(ESyncDomain::Server, ServerJobs))
    {
        return false;
    }

    bool bOk = true;
    if (PlayerJobs.Num() > 0)
    {
        bOk &= Persistence->UpsertPlayerBatch(PlayerJobs);
    }
    if (WorldJobs.Num() > 0)
    {
        bOk &= Persistence->UpsertWorldBatch(WorldJobs);
    }
    if (ServerJobs.Num() > 0)
    {
        bOk &= Persistence->UpsertServerBatch(ServerJobs);
    }

    return bOk;
}

void UDataSyncSchedulerSubsystem::OnPersistDone(bool bSuccess)
{
    if (!bSuccess)
    {
        {
            FScopeLock Lock(&QueueMutex);
            Queue.Append(InFlight);
        }
        InFlight.Reset();

        RetryBackoffIndex = FMath::Clamp(RetryBackoffIndex + 1, 0, 4);
        BackoffTimer = BackoffSteps[RetryBackoffIndex];
    }
    else
    {
        InFlight.Reset();
        RetryBackoffIndex = 0;
        BackoffTimer = 0.f;
    }
}

void UDataSyncSchedulerSubsystem::FlushNow()
{
    if (!DrainQueueToInFlight())
    {
        return;
    }

    const bool bOk = BatchPersist(InFlight);
    OnPersistDone(bOk);

    if (bOk)
    {
        AutosaveTimer = 0.f;
    }
}

bool UDataSyncSchedulerSubsystem::BuildSnapshotJson(TArray<uint8>& OutJsonBytes)
{
    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();

    TArray<FSyncJob> SnapshotQueue;
    {
        FScopeLock Lock(&QueueMutex);
        SnapshotQueue = Queue;
    }

    auto JobsToJson = [](const TArray<FSyncJob>& Jobs)
    {
        TArray<TSharedPtr<FJsonValue>> Values;
        const UEnum* DomainEnum = StaticEnum<ESyncDomain>();
        const UEnum* SensitivityEnum = StaticEnum<ESyncSensitivity>();

        for (const FSyncJob& Job : Jobs)
        {
            TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
            if (DomainEnum)
            {
                Obj->SetStringField(TEXT("domain"), DomainEnum->GetNameStringByValue(static_cast<int64>(Job.Domain)));
            }
            if (SensitivityEnum)
            {
                Obj->SetStringField(TEXT("sensitivity"), SensitivityEnum->GetNameStringByValue(static_cast<int64>(Job.Sensitivity)));
            }
            Obj->SetStringField(TEXT("key"), Job.Key);
            Obj->SetStringField(TEXT("topic"), Job.Topic);
            Obj->SetStringField(TEXT("payload"), Job.JsonPayload);
            Obj->SetBoolField(TEXT("upsert"), Job.bUpsert);
            Obj->SetNumberField(TEXT("version"), static_cast<double>(Job.Version));
            Values.Add(MakeShared<FJsonValueObject>(Obj));
        }

        return Values;
    };

    Root->SetArrayField(TEXT("queued"), JobsToJson(SnapshotQueue));
    Root->SetArrayField(TEXT("inflight"), JobsToJson(InFlight));
    Root->SetNumberField(TEXT("server_time_unix"), static_cast<double>(FDateTime::UtcNow().ToUnixTimestamp()));

    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    if (!FJsonSerializer::Serialize(Root, Writer))
    {
        return false;
    }

    CopyStringToUtf8Bytes(JsonString, OutJsonBytes);
    return true;
}

bool UDataSyncSchedulerSubsystem::WriteSnapshotNow()
{
    TArray<uint8> JsonBytes;
    if (!BuildSnapshotJson(JsonBytes))
    {
        return false;
    }

    FSnapshotMeta Meta;
    const bool bOk = USnapshotManager::WriteSnapshot(JsonBytes, Meta);
    if (bOk)
    {
        SnapshotTimer = 0.f;
    }
    return bOk;
}

bool UDataSyncSchedulerSubsystem::RestoreLatestSnapshot()
{
    TArray<uint8> JsonBytes;
    if (!USnapshotManager::ReadLatest(JsonBytes))
    {
        return false;
    }

    JsonBytes.Add(0);
    const FString JsonString = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(JsonBytes.GetData())));

    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        return false;
    }

    auto ParseJobs = [](const TArray<TSharedPtr<FJsonValue>>* ArrayPtr, TArray<FSyncJob>& OutJobs)
    {
        if (!ArrayPtr)
        {
            return;
        }

        const UEnum* DomainEnum = StaticEnum<ESyncDomain>();
        const UEnum* SensitivityEnum = StaticEnum<ESyncSensitivity>();

        for (const TSharedPtr<FJsonValue>& Value : *ArrayPtr)
        {
            const TSharedPtr<FJsonObject>* ObjectPtr;
            if (!Value.IsValid() || !Value->TryGetObject(ObjectPtr) || !ObjectPtr || !ObjectPtr->IsValid())
            {
                continue;
            }

            const TSharedPtr<FJsonObject>& Object = *ObjectPtr;
            FSyncJob Job;

            FString DomainString;
            if (Object->TryGetStringField(TEXT("domain"), DomainString) && DomainEnum)
            {
                const int64 DomainValue = DomainEnum->GetValueByNameString(DomainString);
                if (DomainValue != INDEX_NONE)
                {
                    Job.Domain = static_cast<ESyncDomain>(DomainValue);
                }
            }

            FString SensitivityString;
            if (Object->TryGetStringField(TEXT("sensitivity"), SensitivityString) && SensitivityEnum)
            {
                const int64 SensitivityValue = SensitivityEnum->GetValueByNameString(SensitivityString);
                if (SensitivityValue != INDEX_NONE)
                {
                    Job.Sensitivity = static_cast<ESyncSensitivity>(SensitivityValue);
                }
            }

            Object->TryGetStringField(TEXT("key"), Job.Key);
            Object->TryGetStringField(TEXT("topic"), Job.Topic);
            Object->TryGetStringField(TEXT("payload"), Job.JsonPayload);
            Object->TryGetBoolField(TEXT("upsert"), Job.bUpsert);

            double VersionValue = 0.0;
            if (Object->TryGetNumberField(TEXT("version"), VersionValue))
            {
                Job.Version = static_cast<int64>(VersionValue);
            }

            OutJobs.Add(Job);
        }
    };

    TArray<FSyncJob> NewInFlight;
    TArray<FSyncJob> NewQueue;

    const TArray<TSharedPtr<FJsonValue>>* InFlightArray = nullptr;
    Root->TryGetArrayField(TEXT("inflight"), InFlightArray);
    ParseJobs(InFlightArray, NewInFlight);

    const TArray<TSharedPtr<FJsonValue>>* QueueArray = nullptr;
    Root->TryGetArrayField(TEXT("queued"), QueueArray);
    ParseJobs(QueueArray, NewQueue);

    {
        FScopeLock Lock(&QueueMutex);
        Queue = MoveTemp(NewQueue);
    }

    InFlight = MoveTemp(NewInFlight);
    return true;
}

void USyncPublisherComponent::EnqueuePlayer(const FString& CharacterId, const FString& Topic, const FString& Json, bool bSensitive, int64 Version)
{
    if (UWorld* World = GetWorld())
    {
        if (UDataSyncSchedulerSubsystem* Scheduler = World->GetSubsystem<UDataSyncSchedulerSubsystem>())
        {
            FSyncJob Job;
            Job.Domain = ESyncDomain::Player;
            Job.Key = CharacterId;
            Job.Topic = Topic;
            Job.JsonPayload = Json;
            Job.Version = Version;
            Job.Sensitivity = bSensitive ? ESyncSensitivity::Sensitive : ESyncSensitivity::Normal;
            Scheduler->EnqueueJob(Job);
        }
    }
}

void USyncPublisherComponent::EnqueueWorld(const FString& RegionId, const FString& Topic, const FString& Json, bool bSensitive, int64 Version)
{
    if (UWorld* World = GetWorld())
    {
        if (UDataSyncSchedulerSubsystem* Scheduler = World->GetSubsystem<UDataSyncSchedulerSubsystem>())
        {
            FSyncJob Job;
            Job.Domain = ESyncDomain::World;
            Job.Key = RegionId;
            Job.Topic = Topic;
            Job.JsonPayload = Json;
            Job.Version = Version;
            Job.Sensitivity = bSensitive ? ESyncSensitivity::Sensitive : ESyncSensitivity::Normal;
            Scheduler->EnqueueJob(Job);
        }
    }
}

void USyncPublisherComponent::EnqueueServer(const FString& ServerId, const FString& Topic, const FString& Json, bool bSensitive, int64 Version)
{
    if (UWorld* World = GetWorld())
    {
        if (UDataSyncSchedulerSubsystem* Scheduler = World->GetSubsystem<UDataSyncSchedulerSubsystem>())
        {
            FSyncJob Job;
            Job.Domain = ESyncDomain::Server;
            Job.Key = ServerId;
            Job.Topic = Topic;
            Job.JsonPayload = Json;
            Job.Version = Version;
            Job.Sensitivity = bSensitive ? ESyncSensitivity::Sensitive : ESyncSensitivity::Normal;
            Scheduler->EnqueueJob(Job);
        }
    }
}

