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
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FCharacterIdentity.h"
#include "FCharacterStats.h"
#include "CharacterSaveSubsystem.generated.h"

UCLASS()
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
    UFUNCTION(BlueprintCallable, Category="Character|Save")
    bool ValidateCharacterName(const FString& FirstName, const FString& LastName, FString& OutError) const;

    UFUNCTION(BlueprintCallable, Category="Character|Save")
    FCharacterIdentity CreateNewCharacter(const FString& FirstName, const FString& LastName, EClassType ClassType, EElementType ElementAffinity);

    UFUNCTION(BlueprintCallable, Category="Character|Save")
    void SaveCharacter(const FCharacterIdentity& Identity, const FCharacterStats& Stats);

    UFUNCTION(BlueprintCallable, Category="Character|Save")
    bool LoadCharacter(const FString& CharacterID, FCharacterIdentity& OutIdentity, FCharacterStats& OutStats) const;

private:
    bool IsValidName(const FString& Name) const;
};
