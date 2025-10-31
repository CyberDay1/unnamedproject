#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "HAL/CriticalSection.h"
#include "Server/SyncJobTypes.h"
#include "DataSyncSchedulerSubsystem.generated.h"

class UPersistenceManagerSubsystem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class USyncPublisherComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Sync")
    void EnqueuePlayer(const FString& CharacterId, const FString& Topic, const FString& Json, bool bSensitive = true, int64 Version = 0);

    UFUNCTION(BlueprintCallable, Category = "Sync")
    void EnqueueWorld(const FString& RegionId, const FString& Topic, const FString& Json, bool bSensitive = true, int64 Version = 0);

    UFUNCTION(BlueprintCallable, Category = "Sync")
    void EnqueueServer(const FString& ServerId, const FString& Topic, const FString& Json, bool bSensitive = true, int64 Version = 0);
};

UCLASS()
class UDataSyncSchedulerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
    virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
    virtual bool IsTickable() const override { return true; }
    virtual bool IsTickableInEditor() const override { return true; }
    virtual TStatId GetStatId() const override;

    UFUNCTION(BlueprintCallable, Category = "Sync")
    void FlushNow();

    UFUNCTION(BlueprintCallable, Category = "Sync")
    bool WriteSnapshotNow();

    UFUNCTION(BlueprintCallable, Category = "Sync")
    bool RestoreLatestSnapshot();

    void EnqueueJob(const FSyncJob& Job);

private:
    bool DrainQueueToInFlight();
    bool BatchPersist(TArray<FSyncJob>& Batch);
    void OnPersistDone(bool bSuccess);
    bool BuildSnapshotJson(TArray<uint8>& OutJsonBytes);

    FCriticalSection QueueMutex;
    TArray<FSyncJob> Queue;
    TArray<FSyncJob> InFlight;

    float AutosaveTimer = 0.f;
    float SnapshotTimer = 0.f;
    float AutosaveInterval = 30.f;
    float SnapshotInterval = 300.f;

    int32 RetryBackoffIndex = 0;
    const float BackoffSteps[5] = {5.f, 10.f, 20.f, 30.f, 60.f};
    float BackoffTimer = 0.f;

    FString SnapshotDir;
    int32 SnapshotKeep = 24;

    TWeakObjectPtr<UPersistenceManagerSubsystem> Persistence;
};

