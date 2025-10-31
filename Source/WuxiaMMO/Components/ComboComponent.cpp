#include "Components/ComboComponent.h"

UComboComponent::UComboComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UComboComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UComboComponent::StartCombo(FName SkillID, FName NextExpected)
{
    CurrentSkill = SkillID;
    ExpectedNext = NextExpected;
    RemainingWindow = ComboWindow;
    ChainCount = 1;
    OnComboProgress.Broadcast(CurrentSkill, ExpectedNext, RemainingWindow);
}

void UComboComponent::TryContinueCombo(FName SkillID, FName NextExpectedOfSkill)
{
    if (ExpectedNext == NAME_None || SkillID != ExpectedNext)
    {
        BreakCombo();
        return;
    }

    ChainCount++;
    CurrentSkill = SkillID;
    ExpectedNext = NextExpectedOfSkill;
    RemainingWindow = ComboWindow;

    OnComboSuccess.Broadcast(ChainCount);
    OnComboProgress.Broadcast(CurrentSkill, ExpectedNext, RemainingWindow);
}

void UComboComponent::BreakCombo()
{
    if (!IsInCombo() && ExpectedNext == NAME_None && ChainCount == 0)
    {
        return;
    }

    CurrentSkill = NAME_None;
    ExpectedNext = NAME_None;
    RemainingWindow = 0.f;
    ChainCount = 0;

    OnComboReset.Broadcast();
}

void UComboComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (RemainingWindow > 0.f)
    {
        RemainingWindow -= DeltaTime;
        if (RemainingWindow <= 0.f)
        {
            BreakCombo();
        }
        else
        {
            OnComboProgress.Broadcast(CurrentSkill, ExpectedNext, RemainingWindow);
        }
    }
}
