#pragma once
#include "CoreMinimal.h"
#include "AIEnums.h"
#include "EnvTypes.h"
#include "AITypes.generated.h"

USTRUCT(BlueprintType)
struct FAIHomeTerritory
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Center = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Radius = 1200.f; // leash distance
};

USTRUCT(BlueprintType)
struct FGuardTownRule
{
    GENERATED_BODY()
    // Town region ID; players of other factions slowly accrue suspicion here
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName TownRegionID = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SuspicionPerSecond = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SuspicionThreshold = 100.0f;
};

USTRUCT(BlueprintType)
struct FAIStats
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float FleeHPThreshold = 0.15f; // percent
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float EngageRange = 1800.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SightRadius = 2200.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LoseSightRadius = 2600.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SleepStartHour = 22.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float WakeHour = 6.f;
};
