#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/FCharacterIdentity.h"
#include "Data/FCharacterStats.h"
#include "CharacterSaveSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FStoredCharacterData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Character")
    FCharacterIdentity Identity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Character")
    FCharacterStats Stats;
};

UCLASS(Config=Game, DefaultConfig)
class UCharacterSaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Character|Save")
    bool LoadCharacter(const FString& CharacterID, FCharacterIdentity& OutIdentity, FCharacterStats& OutStats) const;

    UFUNCTION(BlueprintCallable, Category="Character|Save")
    void RegisterCharacter(const FStoredCharacterData& CharacterData);

private:
    void CacheConfiguredCharacters();

    UPROPERTY(Config)
    TArray<FStoredCharacterData> DefaultCharacterSaves;

    UPROPERTY(Transient)
    TMap<FString, FStoredCharacterData> CachedCharacters;
};
