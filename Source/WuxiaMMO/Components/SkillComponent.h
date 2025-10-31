#pragma once

#include "CoreMinimal.h"
#include "Combat/DamageTypes.h"
#include "Components/ActorComponent.h"
#include "Data/FSkillDefinition.h"
#include "Data/FClassBalanceData.h"
#include "SkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillUsed, FName, SkillID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillLevelUp, FName, SkillID, int32, NewLevel);

USTRUCT(BlueprintType)
struct FActiveSkillData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSkillDefinition Definition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CurrentXP = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CooldownRemaining = 0.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class USkillComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USkillComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skills")
    TMap<FName, FActiveSkillData> KnownSkills;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skills")
    TArray<FClassBalanceData> ClassBalanceTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
    bool bLastInputWasKeyCombo = false;

    UFUNCTION(BlueprintCallable, Category="Input")
    void RegisterInputSource(bool bIsKeyCombo) { bLastInputWasKeyCombo = bIsKeyCombo; }

    UFUNCTION(BlueprintCallable, Category="Skills")
    bool UseSkill(FName SkillID, float CurrentQi, float& OutQiCost, float& OutDamage);

    UFUNCTION(BlueprintCallable, Category="Skills|Combat")
    FDamageResult ApplySkillDamageToTarget(FName SkillID, AActor* TargetActor, float BaseDamage);

    UFUNCTION(BlueprintCallable, Category="Skills")
    void AddSkillXP(FName SkillID, float Amount, ECultivationStage PlayerRealm);

    UFUNCTION(BlueprintCallable, Category="Skills")
    void TickCooldowns(float DeltaSeconds);

    UPROPERTY(BlueprintAssignable)
    FOnSkillUsed OnSkillUsed;

    UPROPERTY(BlueprintAssignable)
    FOnSkillLevelUp OnSkillLevelUp;

private:
    bool CanUseSkill(const FActiveSkillData& Skill, float CurrentQi, float ActualQiCost) const;
    void ApplyCooldown(FActiveSkillData& Skill);
    float CalculateDamage(const FSkillDefinition& Skill, const FClassBalanceData& ClassData, const TMap<FString, float>& Stats) const;
};
