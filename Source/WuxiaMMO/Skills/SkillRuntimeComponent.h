#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "SkillTypes.h"
#include "SkillRuntimeComponent.generated.h"

class UCombatComponent;
class USkillMasteryComponent;
class UCultivationComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSkillCast, FName, SkillID, AActor*, Caster, const FSkillDefinition&, Def);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSkillImpact, FName, SkillID, AActor*, Target, const FSkillDefinition&, Def);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillEnd, FName, SkillID, const FSkillDefinition&, Def);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class USkillRuntimeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSet<FName> KnownSkills;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, double> CooldownUntil;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UDataTable* RuntimeSkillTableOverride = nullptr;

    UPROPERTY(BlueprintAssignable)
    FOnSkillCast OnSkillCast;

    UPROPERTY(BlueprintAssignable)
    FOnSkillImpact OnSkillImpact;

    UPROPERTY(BlueprintAssignable)
    FOnSkillEnd OnSkillEnd;

    UFUNCTION(BlueprintCallable)
    bool LearnSkill(FName SkillID);

    UFUNCTION(BlueprintCallable)
    bool UseSkillByID(FName SkillID);

    UFUNCTION(BlueprintPure)
    bool IsOnCooldown(FName SkillID) const;

    UFUNCTION(BlueprintCallable)
    void ForceCooldown(FName SkillID, float Seconds);

    UFUNCTION(BlueprintPure)
    void GetKnownSkills(TArray<FName>& OutSkillIDs) const;

    UFUNCTION(BlueprintPure)
    float GetCooldownRemaining(FName SkillID) const;

private:
    bool GetDef(FName SkillID, FSkillDefinition& Out) const;
    bool PassesUnlocks(const FSkillDefinition& Def) const;
    bool SpendQi(const FSkillDefinition& Def);
    double Now() const;
};

