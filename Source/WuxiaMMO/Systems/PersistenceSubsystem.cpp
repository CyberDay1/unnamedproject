#include "Systems/PersistenceSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameInstance.h"
#include "HAL/FileManager.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Systems/CharacterSaveSubsystem.h"
#include "Systems/PersistenceHttpClient.h"
#include "Data/Save/SaveEncryption.h"
#include "Config/PersistenceConfig.h"
#include "Components/CultivationComponent.h"
#include "Components/SpatialRingComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/QuestComponent.h"
#include "Components/ReputationComponent.h"
#include "Components/CurrencyComponent.h"
#include "Components/SkillComponent.h"
#include "JsonObjectConverter.h"
#include "Containers/StringConv.h"

namespace
{
    bool ExtractPayload(const FString& Response, FString& OutPayload)
    {
        TSharedPtr<FJsonObject> Object;
        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
        if (!FJsonSerializer::Deserialize(Reader, Object) || !Object.IsValid())
        {
            return false;
        }

        if (!Object->TryGetStringField(TEXT("payload"), OutPayload))
        {
            return false;
        }
        return true;
    }
}

void UPersistenceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    HttpClient = NewObject<UPersistenceHttpClient>(this);
    AESKey = FPersistenceConfig::DefaultAESKey();
    LastFlushTime = FPlatformTime::Seconds();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(SaveTickHandle, this, &UPersistenceSubsystem::TickSaveQueue, 1.0f, true);
    }
}

void UPersistenceSubsystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SaveTickHandle);
    }

    Super::Deinitialize();
}

void UPersistenceSubsystem::SetAESKey(const TArray<uint8>& InKey)
{
    AESKey = InKey;
}

void UPersistenceSubsystem::RequestLoad(const FString& AccountID, const FString& CharacterID, TFunction<void(bool, const FPlayerProfile&)> Callback)
{
    const auto InvokeCallback = [Callback](bool bSuccess, const FPlayerProfile& Profile)
    {
        if (Callback)
        {
            Callback(bSuccess, Profile);
        }
    };

    FPlayerProfile RequestPayload;
    RequestPayload.AccountID = AccountID;
    RequestPayload.CharacterID = CharacterID;

    FString EncryptedRequest;
    if (!ProfileToEncryptedBase64(RequestPayload, EncryptedRequest))
    {
        InvokeCallback(false, FPlayerProfile{});
        return;
    }

    const auto HandleDecodedProfile = [this, InvokeCallback](const FString& Base64Payload)
    {
        FPlayerProfile LoadedProfile;
        if (!Base64Payload.IsEmpty() && EncryptedBase64ToProfile(Base64Payload, LoadedProfile))
        {
            PruneCooldowns(LoadedProfile);
            InvokeCallback(true, LoadedProfile);
        }
        else
        {
            InvokeCallback(false, FPlayerProfile{});
        }
    };

    const auto AttemptLocal = [this, CharacterID, HandleDecodedProfile, InvokeCallback]()
    {
        FString LocalBase64;
        if (LoadLocal(CharacterID, LocalBase64))
        {
            HandleDecodedProfile(LocalBase64);
        }
        else
        {
            InvokeCallback(false, FPlayerProfile{});
        }
    };

    if (!HttpClient)
    {
        AttemptLocal();
        return;
    }

    HttpClient->PostLoad(FPersistenceConfig::LoadUrl(), EncryptedRequest, FOnPersistenceResponse::CreateLambda(
        [HandleDecodedProfile, AttemptLocal](bool bOk, const FString& ResponseBody)
        {
            if (!bOk)
            {
                AttemptLocal();
                return;
            }

            FString PayloadString;
            if (!ExtractPayload(ResponseBody, PayloadString))
            {
                AttemptLocal();
                return;
            }

            HandleDecodedProfile(PayloadString);
        }));
}

void UPersistenceSubsystem::QueueSave(const FPlayerProfile& Profile, const FString& ReasonTag)
{
    LastQueuedProfile = Profile;
    LastReason = ReasonTag;
    bSaveQueued = true;
    LastQueueTime = FPlatformTime::Seconds();
}

void UPersistenceSubsystem::FlushSavesNow()
{
    if (!bSaveQueued)
    {
        return;
    }

    FString EncryptedPayload;
    if (!ProfileToEncryptedBase64(LastQueuedProfile, EncryptedPayload))
    {
        return;
    }

    if (HttpClient)
    {
        HttpClient->PostSave(FPersistenceConfig::SaveUrl(), EncryptedPayload, FOnPersistenceResponse::CreateLambda(
            [](bool /*bOk*/, const FString& /*Response*/)
            {
            }
        ));
    }

    SaveLocal(LastQueuedProfile.CharacterID, EncryptedPayload);

    bSaveQueued = false;
    LastFlushTime = FPlatformTime::Seconds();
    LastQueueTime = 0.0;
    LastReason.Reset();
}

bool UPersistenceSubsystem::SaveLocal(const FString& CharacterID, const FString& EncryptedBase64)
{
    const FString Directory = FPaths::ProjectSavedDir() / TEXT("Profiles");
    IFileManager::Get().MakeDirectory(*Directory, true);
    const FString FilePath = Directory / (CharacterID + TEXT(".profile"));
    return FFileHelper::SaveStringToFile(EncryptedBase64, *FilePath);
}

bool UPersistenceSubsystem::LoadLocal(const FString& CharacterID, FString& OutEncryptedBase64)
{
    const FString FilePath = FPaths::ProjectSavedDir() / TEXT("Profiles") / (CharacterID + TEXT(".profile"));
    return FFileHelper::LoadFileToString(OutEncryptedBase64, *FilePath);
}

void UPersistenceSubsystem::TickSaveQueue()
{
    if (!bSaveQueued)
    {
        return;
    }

    const double Now = FPlatformTime::Seconds();
    const bool bPastRateLimit = (Now - LastFlushTime) >= FPersistenceConfig::MinSecondsBetweenSaves;
    const bool bQueuedLongEnough = (Now - LastQueueTime) >= FPersistenceConfig::MinSecondsBetweenSaves;
    const bool bForceFlush = LastQueueTime > 0.0 && (Now - LastQueueTime) >= FPersistenceConfig::MaxSecondsUntilForced;

    if ((bPastRateLimit && bQueuedLongEnough) || bForceFlush)
    {
        FlushSavesNow();
    }
}

bool UPersistenceSubsystem::ProfileToEncryptedBase64(const FPlayerProfile& Profile, FString& OutBase64) const
{
    if (AESKey.Num() != 32)
    {
        return false;
    }

    FString JsonString;
    if (!FJsonObjectConverter::UStructToJsonObjectString(Profile, JsonString))
    {
        return false;
    }

    FTCHARToUTF8 Converter(*JsonString);
    TArray<uint8> Bytes;
    Bytes.Append(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length());

    return SaveCrypto::EncryptToBase64(Bytes, AESKey, OutBase64);
}

bool UPersistenceSubsystem::EncryptedBase64ToProfile(const FString& Base64, FPlayerProfile& OutProfile) const
{
    if (AESKey.Num() != 32)
    {
        return false;
    }

    TArray<uint8> PlainBytes;
    if (!SaveCrypto::DecryptFromBase64(Base64, AESKey, PlainBytes))
    {
        return false;
    }

    FUTF8ToTCHAR Converter(reinterpret_cast<const ANSICHAR*>(PlainBytes.GetData()), PlainBytes.Num());
    const FString JsonString(Converter.Length(), Converter.Get());

    return FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &OutProfile, 0, 0);
}

void UPersistenceSubsystem::PruneCooldowns(FPlayerProfile& Profile) const
{
    const double Now = FPlatformTime::Seconds();
    Profile.Cooldowns.RemoveAll([Now](const FCooldownSnapshot& Cooldown)
    {
        return (Now - Cooldown.StartTime) > FPersistenceConfig::CooldownTTLSeconds;
    });
}

FPlayerProfile UPersistenceSubsystem::GatherProfileSnapshot(AActor* PlayerActor, const FString& AccountID, const FString& CharacterID) const
{
    FPlayerProfile Profile;
    Profile.AccountID = AccountID;
    Profile.CharacterID = CharacterID;

    if (UCharacterSaveSubsystem* CharacterSave = GetGameInstance()->GetSubsystem<UCharacterSaveSubsystem>())
    {
        CharacterSave->LoadCharacter(CharacterID, Profile.Identity, Profile.Stats);
    }

    if (!PlayerActor)
    {
        return Profile;
    }

    if (const UCultivationComponent* Cultivation = PlayerActor->FindComponentByClass<UCultivationComponent>())
    {
        Profile.Cultivation = Cultivation->Progress;
    }

    if (const USpatialRingComponent* SpatialRing = PlayerActor->FindComponentByClass<USpatialRingComponent>())
    {
        Profile.InventoryItems = SpatialRing->StoredItems;
    }

    if (const UEquipmentComponent* Equipment = PlayerActor->FindComponentByClass<UEquipmentComponent>())
    {
        Profile.Equipment.Equipped = Equipment->EquippedItems;
    }

    if (const UQuestComponent* QuestComponent = PlayerActor->FindComponentByClass<UQuestComponent>())
    {
        Profile.ActiveQuests = QuestComponent->ActiveQuests;
    }

    if (const UReputationComponent* ReputationComponent = PlayerActor->FindComponentByClass<UReputationComponent>())
    {
        Profile.Reputation.FactionReputation = ReputationComponent->FactionReputation;
        Profile.Reputation.SectReputations = ReputationComponent->SectReps;
    }

    if (const UCurrencyComponent* CurrencyComponent = PlayerActor->FindComponentByClass<UCurrencyComponent>())
    {
        Profile.Currency.Balances = CurrencyComponent->Balances;
    }

    if (const USkillComponent* SkillComponent = PlayerActor->FindComponentByClass<USkillComponent>())
    {
        for (const TPair<FName, FActiveSkillData>& Pair : SkillComponent->KnownSkills)
        {
            Profile.Skills.SkillLevels.Add(Pair.Key, Pair.Value.CurrentLevel);
            Profile.Skills.SkillXP.Add(Pair.Key, Pair.Value.CurrentXP);
        }
    }

    return Profile;
}

void UPersistenceSubsystem::ApplyProfileToWorld(const FPlayerProfile& Profile, AActor* PlayerActor) const
{
    if (!PlayerActor)
    {
        return;
    }

    if (UCultivationComponent* Cultivation = PlayerActor->FindComponentByClass<UCultivationComponent>())
    {
        Cultivation->Progress = Profile.Cultivation;
        Cultivation->RecomputeRequirement();
    }

    if (USpatialRingComponent* SpatialRing = PlayerActor->FindComponentByClass<USpatialRingComponent>())
    {
        SpatialRing->StoredItems = Profile.InventoryItems;
        SpatialRing->UpdateSlotCount(static_cast<int32>(Profile.Cultivation.SubLevel));
    }

    if (UEquipmentComponent* Equipment = PlayerActor->FindComponentByClass<UEquipmentComponent>())
    {
        Equipment->EquippedItems = Profile.Equipment.Equipped;
    }

    if (UQuestComponent* QuestComponent = PlayerActor->FindComponentByClass<UQuestComponent>())
    {
        QuestComponent->ActiveQuests = Profile.ActiveQuests;
    }

    if (UReputationComponent* ReputationComponent = PlayerActor->FindComponentByClass<UReputationComponent>())
    {
        ReputationComponent->FactionReputation = Profile.Reputation.FactionReputation;
        ReputationComponent->SectReps = Profile.Reputation.SectReputations;
    }

    if (UCurrencyComponent* CurrencyComponent = PlayerActor->FindComponentByClass<UCurrencyComponent>())
    {
        CurrencyComponent->Balances = Profile.Currency.Balances;
    }

    if (USkillComponent* SkillComponent = PlayerActor->FindComponentByClass<USkillComponent>())
    {
        for (TPair<FName, FActiveSkillData>& Pair : SkillComponent->KnownSkills)
        {
            if (const int32* Level = Profile.Skills.SkillLevels.Find(Pair.Key))
            {
                Pair.Value.CurrentLevel = *Level;
            }

            if (const float* XP = Profile.Skills.SkillXP.Find(Pair.Key))
            {
                Pair.Value.CurrentXP = *XP;
            }
        }
    }
}
