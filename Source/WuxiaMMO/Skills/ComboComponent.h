#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillTypes.h"
#include "ComboComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnComboProgress, FName, CurrentSkill, FName, NextExpected, float, TimeRemaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboSuccess, int32, ComboCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComboReset);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UComboComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UComboComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combo")
    float ComboWindow = 1.0f;

    UPROPERTY(BlueprintAssignable, Category="Combo")
    FOnComboProgress OnComboProgress;

    UPROPERTY(BlueprintAssignable, Category="Combo")
    FOnComboSuccess OnComboSuccess;

    UPROPERTY(BlueprintAssignable, Category="Combo")
    FOnComboReset OnComboReset;

    UFUNCTION(BlueprintCallable, Category="Combo")
    void StartCombo(FName SkillID, const FSkillDefinition& Def);

    UFUNCTION(BlueprintCallable, Category="Combo")
    void TryContinueCombo(FName SkillID, const FSkillDefinition& Def);

    UFUNCTION(BlueprintCallable, Category="Combo")
    void BreakCombo();

    UFUNCTION(BlueprintPure, Category="Combo")
    bool IsInCombo() const { return RemainingWindow > 0.f; }

    UFUNCTION(BlueprintPure, Category="Combo")
    int32 GetChainCount() const { return ChainCount; }

    UFUNCTION(BlueprintPure, Category="Combo")
    FName GetExpectedNext() const { return ExpectedNext; }

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    FName CurrentSkill = NAME_None;
    FName ExpectedNext = NAME_None;
    float RemainingWindow = 0.f;
    int32 ChainCount = 0;
};
