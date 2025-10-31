#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "CombatTypes.h"
#include "CombatComponent.generated.h"

class UHealthComponent;
class UReputationComponent;
class UCultivationComponent;
class UWeatherSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHitTarget, AActor*, Target, float, Damage, bool, bIsSkill, FName, SkillID);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    UDataTable* SkillDataTable = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float BasicAttackBase = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float BasicAttackMult = 0.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float BasicRange = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float BasicRadius = 60.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    TMap<FName, double> CooldownMap;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PvP")
    bool bAllowPvP = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PvP")
    bool bInSafeZone = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PvP")
    bool bDuelOnlyWithConsent = true;

    UPROPERTY(BlueprintAssignable)
    FOnHitTarget OnHitTarget;

    UFUNCTION(BlueprintCallable, Category="Combat")
    bool PerformBasicAttack(AActor* PreferredTarget);

    UFUNCTION(BlueprintCallable, Category="Combat")
    bool PerformSkillByID(FName SkillID, AActor* PreferredTarget);

    UFUNCTION(BlueprintPure, Category="Combat")
    bool IsSkillReady(FName SkillID) const;

    UFUNCTION(BlueprintCallable, Category="Combat")
    void SetCooldown(FName SkillID, float Seconds);

private:
    bool GetSkillRow(FName SkillID, FSkillRow& OutRow) const;
    bool SphereTraceForTarget(const FVector& Center, float Radius, float Range, AActor*& OutTarget) const;
    bool CanDamageTarget(AActor* Target, bool bIsSkill) const;
    float ComputeGlobalAttackMultiplier(bool bIsMonster) const;
    float GetAttackerScalingStat() const;
    float GetTargetDefense(const AActor* Target) const;
    float GetElementAffinityBonus(EElementType Elem) const;
};
