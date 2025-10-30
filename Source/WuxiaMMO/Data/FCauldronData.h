#pragma once
#include "CoreMinimal.h"
#include "FCauldronData.generated.h"

USTRUCT(BlueprintType)
struct FCauldronData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cauldron")
    FName CauldronID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cauldron")
    int32 Tier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cauldron")
    float Durability = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cauldron")
    float MaxDurability = 100.0f;

    // Passive bonuses while crafting with this cauldron
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cauldron")
    float BonusQualityPct = 0.0f;  // 0.10 = +10% quality

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cauldron")
    float BonusSuccessPct = 0.0f;  // 0.05 = +5% success
};
