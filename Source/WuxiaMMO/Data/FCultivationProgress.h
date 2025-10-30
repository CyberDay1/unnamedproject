#pragma once
#include "CoreMinimal.h"
#include "ECultivationStage.h"
#include "FCultivationProgress.generated.h"

USTRUCT(BlueprintType)
struct FCultivationProgress
{
    GENERATED_BODY()

    // Current realm and sub-level (1–9)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation")
    ECultivationStage Stage = ECultivationStage::MortalBody;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation")
    uint8 SubLevel = 1; // 1..9

    // Qi storage toward next sub-level
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation")
    float CurrentQi = 0.0f;

    // Computed requirement for the current (Stage, SubLevel)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation")
    float RequiredQi = 100.0f;

    // Per-pill-name efficiency penalty: 1% less per use of SAME pill name
    // Key = PillName, Value = TimesConsumed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation")
    TMap<FName, int32> PillEfficiencyCounters;

    // Block additional Qi gain when SubLevel==9 and CurrentQi>=RequiredQi until breakthrough
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation")
    bool bQiGainBlockedAtCap = false;
};
