#pragma once

#include "CoreMinimal.h"
#include "WuxiaDataTypes.generated.h"

UENUM(BlueprintType)
enum class EWuxiaDamageType : uint8
{
    Physical UMETA(DisplayName="Physical"),
    Elemental UMETA(DisplayName="Elemental"),
    Internal UMETA(DisplayName="Internal")
};

USTRUCT(BlueprintType)
struct FWuxiaCombatState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    bool bIsInCombat = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float TimeSinceCombat = 0.f;
};
