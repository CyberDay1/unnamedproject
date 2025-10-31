#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillBarWidget.generated.h"

class USkillRuntimeComponent;

UCLASS()
class USkillBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Skills")
    void InitializeSkillBar(USkillRuntimeComponent* InComponent);

    UFUNCTION(BlueprintCallable, Category="Skills")
    void RefreshFromComponent();

    UFUNCTION(BlueprintCallable, Category="Skills")
    bool TriggerSkillByIndex(int32 Index);

    UFUNCTION(BlueprintPure, Category="Skills")
    const TArray<FName>& GetDisplayedSkills() const { return DisplayedSkills; }

    UFUNCTION(BlueprintPure, Category="Skills")
    float GetCooldownForSkill(FName SkillID) const;

private:
    UPROPERTY(Transient)
    TObjectPtr<USkillRuntimeComponent> RuntimeComponent;

    UPROPERTY(Transient)
    TArray<FName> DisplayedSkills;
};

