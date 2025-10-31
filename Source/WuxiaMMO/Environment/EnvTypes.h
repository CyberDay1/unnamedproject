#pragma once
#include "CoreMinimal.h"
#include "EnvEnums.h"
#include "EnvTypes.generated.h"

USTRUCT(BlueprintType)
struct FEnvironmentRegion
{
    GENERATED_BODY()

    // Region identifier (can be a level/streaming cell/volume tag)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RegionID = NAME_None;

    // Optional world-space center/radius for local weather; leave Radius<=0 for global
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Center = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius = 0.f;
};

USTRUCT(BlueprintType)
struct FEnvironmentModifiers
{
    GENERATED_BODY()

    // Multipliers (1.0 = no change)
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float QiRegenMult        = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MeditationMult     = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SpawnRateMult      = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MonsterAtkMult     = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MonsterDefMult     = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LeashRadiusMult    = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float GatheringYieldMult = 1.0f;

    static FEnvironmentModifiers Combine(const FEnvironmentModifiers& A, const FEnvironmentModifiers& B)
    {
        FEnvironmentModifiers R;
        R.QiRegenMult        = A.QiRegenMult        * B.QiRegenMult;
        R.MeditationMult     = A.MeditationMult     * B.MeditationMult;
        R.SpawnRateMult      = A.SpawnRateMult      * B.SpawnRateMult;
        R.MonsterAtkMult     = A.MonsterAtkMult     * B.MonsterAtkMult;
        R.MonsterDefMult     = A.MonsterDefMult     * B.MonsterDefMult;
        R.LeashRadiusMult    = A.LeashRadiusMult    * B.LeashRadiusMult;
        R.GatheringYieldMult = A.GatheringYieldMult * B.GatheringYieldMult;
        return R;
    }
};

USTRUCT(BlueprintType)
struct FActiveWeather
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FEnvironmentRegion Region;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) EWeatherType Type = EWeatherType::Sunny;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float StartTime = 0.f; // world seconds
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Duration  = 0.f; // seconds; 0=until cleared
};
