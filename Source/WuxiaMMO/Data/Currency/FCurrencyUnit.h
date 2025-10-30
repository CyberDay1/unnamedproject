#pragma once

#include "CoreMinimal.h"
#include "Data/Currency/ECurrencyType.h"
#include "Characters/ECultivationStage.h"
#include "FCurrencyUnit.generated.h"

// Meta info for each currency type (conversion and optional cultivation Qi value)
USTRUCT(BlueprintType)
struct FCurrencyUnit
{
    GENERATED_BODY()

    // Conversion chain index (lower = cheaper). SectContribution should be -1.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Currency")
    int32 OrderIndex = -1;

    // Ratio to convert FROM this unit TO the next (e.g., 10 for pre-immortal, 100 for immortal tiers)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Currency")
    int32 RatioToNext = 10;

    // Base Qi amount this unit can provide when used for cultivation (prototype; 0 if N/A)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Currency")
    float BaseQiGrant = 0.0f;

    // Minimum cultivation stage required to fully utilize (stones above tier unusable for cultivation)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Currency")
    ECultivationStage MinStageForFullEffect = ECultivationStage::MortalBody;
};
