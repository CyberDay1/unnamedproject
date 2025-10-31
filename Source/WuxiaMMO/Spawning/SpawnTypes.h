#pragma once
#include "CoreMinimal.h"
#include "EnvTypes.h"
#include "SpawnTypes.generated.h"

UENUM(BlueprintType)
enum class ESpawnTier : uint8
{
    Common,
    Elite,
    Rare,
    Boss
};

USTRUCT(BlueprintType)
struct FSpawnEntry
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<APawn> PawnClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) ESpawnTier Tier = ESpawnTier::Common;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Weight = 1.f; // selection weight
};

USTRUCT(BlueprintType)
struct FSpawnGroup
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName GroupID = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FEnvironmentRegion Region;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FSpawnEntry> Entries;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 PopulationCap = 8;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float RespawnSeconds = 30.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float RespawnVariance = 10.f;

    // Allow special near-player rares (e.g., Treasure Pigs)
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableNearPlayerRares = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float RareCheckInterval = 60.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float RareChance = 0.03f; // 3% per interval per group
};
