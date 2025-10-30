#pragma once
#include "CoreMinimal.h"
#include "FAlchemyModifier.generated.h"

UENUM(BlueprintType)
enum class EAlchemyModifierType : uint8
{
    QualityBoost,     // +% to resulting quality multiplier
    SuccessBoost,     // +% to craft success chance
    EfficiencyBoost,  // -% to pill efficiency decay rate
    FailureProtect    // Prevents critical failure once
};

USTRUCT(BlueprintType)
struct FAlchemyModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifier")
    EAlchemyModifierType Type = EAlchemyModifierType::SuccessBoost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifier")
    float Magnitude = 0.0f; // e.g., 0.10 = +10%

    // Optional charges for single-use protection, or duration seconds for timed buffs (unused in prototype)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifier")
    int32 Uses = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifier")
    float DurationSeconds = 0.0f;
};
