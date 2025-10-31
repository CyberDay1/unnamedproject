#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.h"
#include "DamageCalculation.generated.h"

UCLASS()
class UDamageCalculation : public UObject
{
    GENERATED_BODY()

public:
    // Returns final damage after all multipliers
    UFUNCTION(BlueprintPure, Category="Combat")
    static float ComputeDamage(const FSkillRow& Skill, float AttackerScalingStat, float ElementAffinityBonus, float GlobalAtkMult, float TargetDefense);

    // Minor elemental adjustment (±5%)
    UFUNCTION(BlueprintPure, Category="Combat")
    static float ElementModifier(EElementType Elem);
};
