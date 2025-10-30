#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimerManager.h"
#include "Data/Save/SaveDTOs.h"
#include "PersistenceSubsystem.generated.h"

class AActor;
class UPersistenceHttpClient;

UCLASS()
class UPersistenceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void SetAESKey(const TArray<uint8>& InKey);

    void RequestLoad(const FString& AccountID, const FString& CharacterID, TFunction<void(bool, const FPlayerProfile&)> Callback);

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void QueueSave(const FPlayerProfile& Profile, const FString& ReasonTag);

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void FlushSavesNow();

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    static bool SaveLocal(const FString& CharacterID, const FString& EncryptedBase64);

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    static bool LoadLocal(const FString& CharacterID, FString& OutEncryptedBase64);

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    FPlayerProfile GatherProfileSnapshot(AActor* PlayerActor, const FString& AccountID, const FString& CharacterID) const;

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void ApplyProfileToWorld(const FPlayerProfile& Profile, AActor* PlayerActor) const;

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:
    bool ProfileToEncryptedBase64(const FPlayerProfile& Profile, FString& OutBase64) const;
    bool EncryptedBase64ToProfile(const FString& Base64, FPlayerProfile& OutProfile) const;
    void PruneCooldowns(FPlayerProfile& Profile) const;
    void TickSaveQueue();

    TArray<uint8> AESKey;

    bool bSaveQueued = false;
    double LastQueueTime = 0.0;
    double LastFlushTime = 0.0;
    FPlayerProfile LastQueuedProfile;
    FString LastReason;

    FTimerHandle SaveTickHandle;

    UPROPERTY()
    UPersistenceHttpClient* HttpClient = nullptr;
};
