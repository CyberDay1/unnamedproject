#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Combat/DamageTypes.h"
#include "DamageResolverSubsystem.generated.h"

class UBuffComponent;
class AActor;

UCLASS()
class UDamageResolverSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Combat")
    FDamageResult ResolveAndApplyDamage(const FDamageSpec& Spec);

    UFUNCTION(BlueprintPure, Category="Combat")
    float ComputeArmorMitigation(float Armor) const;

    UFUNCTION(BlueprintPure, Category="Combat")
    float ComputeElementMitigation(float Resist01) const;

    UFUNCTION(BlueprintPure, Category="Combat")
    float RollCrit(float CritChance, float CritDamageMul) const;

    UFUNCTION(BlueprintPure, Category="Combat")
    bool RollDodge(float HitChance, float DodgeChance) const;

private:
    bool PassesPvPGate(const AActor* Source, const AActor* Target) const;
    float GetElementResist01(const AActor* Target, EElementType Elem) const;
    float GetArmorValue(const AActor* Target) const;
    float GetOffenseStat(const AActor* Source) const;
    float GetCritChance(const AActor* Source) const;
    float GetCritDamageMul(const AActor* Source) const;
    float GetHitChance(const AActor* Source) const;
    float GetDodgeChance(const AActor* Target) const;
};
