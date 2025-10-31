#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffTypes.h"
#include "BuffComponent.generated.h"

class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuffApplied, const FActiveBuff&, Buff, int32, NewStackCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuffRemoved, const FActiveBuff&, Buff);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UBuffComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBuffComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Buffs")
    UDataTable* BuffTable = nullptr;

    UPROPERTY(BlueprintAssignable, Category="Buffs")
    FOnBuffApplied OnBuffApplied;

    UPROPERTY(BlueprintAssignable, Category="Buffs")
    FOnBuffRemoved OnBuffRemoved;

    UFUNCTION(BlueprintCallable, Category="Buffs")
    bool ApplyBuffByID(FName BuffID, int32 Stacks = 1, float OverrideDurationSeconds = -1.f);

    UFUNCTION(BlueprintCallable, Category="Buffs")
    bool RemoveBuffByTag(FGameplayTag BuffTag);

    UFUNCTION(BlueprintPure, Category="Buffs")
    float GetDamageMultiplier() const { return CachedDamageMultiplier; }

    UFUNCTION(BlueprintPure, Category="Buffs")
    float GetQiCostMultiplier() const { return CachedQiCostMultiplier; }

    UFUNCTION(BlueprintPure, Category="Buffs")
    float GetDefenseMultiplier() const { return CachedDefenseMultiplier; }

    UFUNCTION(BlueprintPure, Category="Buffs")
    bool IsCastingBlocked() const { return bCachedBlocksCasting; }

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    virtual void BeginPlay() override;

private:
    bool FindBuffDef(FName BuffID, FBuffDefinition& OutDefinition) const;
    void RecalculateCachedValues();

    UPROPERTY()
    TArray<FActiveBuff> ActiveBuffs;

    float CachedDamageMultiplier = 1.f;
    float CachedQiCostMultiplier = 1.f;
    float CachedDefenseMultiplier = 1.f;
    float CachedMoveSpeedMultiplier = 1.f;
    bool bCachedBlocksCasting = false;
};
