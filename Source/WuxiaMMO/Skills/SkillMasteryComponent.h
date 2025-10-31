#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillTypes.h"
#include "SkillMasteryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillMasteryLevelUp, FName, SkillID, int32, NewLevel);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class USkillMasteryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSkillProgress> Skills;

    UPROPERTY(BlueprintAssignable)
    FOnSkillMasteryLevelUp OnLevelUp;

    UFUNCTION(BlueprintCallable)
    void GrantXP(FName SkillID, float Amount);

    UFUNCTION(BlueprintCallable)
    int32 GetLevel(FName SkillID) const;

    UFUNCTION(BlueprintCallable)
    float GetLevelDamageBonus(FName SkillID, const FSkillDefinition& Def) const;

    UFUNCTION(BlueprintCallable)
    float GetQiCostMultiplier(FName SkillID, const FSkillDefinition& Def) const;

    UFUNCTION(BlueprintCallable)
    float GetCooldownMultiplier(FName SkillID, const FSkillDefinition& Def) const;

private:
    FSkillProgress* FindOrAdd(FName SkillID);
};

