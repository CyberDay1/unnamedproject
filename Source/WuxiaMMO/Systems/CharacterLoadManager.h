#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/FCharacterIdentity.h"
#include "Data/FCharacterStats.h"
#include "CharacterLoadManager.generated.h"

class ACharacter;

UCLASS()
class UCharacterLoadManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Character|Load")
    ACharacter* SpawnCharacterFromSave(const FString& CharacterID, const FVector& SpawnLocation, const FRotator& SpawnRotation);
};
