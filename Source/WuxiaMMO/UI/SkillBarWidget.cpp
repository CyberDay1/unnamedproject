#include "UI/SkillBarWidget.h"
#include "Skills/SkillRuntimeComponent.h"

void USkillBarWidget::InitializeSkillBar(USkillRuntimeComponent* InComponent)
{
    RuntimeComponent = InComponent;
    RefreshFromComponent();
}

void USkillBarWidget::RefreshFromComponent()
{
    if (RuntimeComponent)
    {
        RuntimeComponent->GetKnownSkills(DisplayedSkills);
    }
    else
    {
        DisplayedSkills.Reset();
    }
}

bool USkillBarWidget::TriggerSkillByIndex(int32 Index)
{
    if (!RuntimeComponent)
    {
        return false;
    }

    if (!DisplayedSkills.IsValidIndex(Index))
    {
        return false;
    }

    return RuntimeComponent->UseSkillByID(DisplayedSkills[Index]);
}

float USkillBarWidget::GetCooldownForSkill(FName SkillID) const
{
    return RuntimeComponent ? RuntimeComponent->GetCooldownRemaining(SkillID) : 0.f;
}

