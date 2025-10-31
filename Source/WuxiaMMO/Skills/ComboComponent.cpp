#include "Skills/ComboComponent.h"

#include "Buffs/BuffComponent.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"

UComboComponent::UComboComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UComboComponent::StartCombo(FName SkillID, const FSkillDefinition& Def)
{
    CurrentSkill = SkillID;
    ExpectedNext = Def.ComboTo;
    RemainingWindow = ComboWindow;
    ChainCount = 1;
    OnComboProgress.Broadcast(CurrentSkill, ExpectedNext, RemainingWindow);
}

void UComboComponent::TryContinueCombo(FName SkillID, const FSkillDefinition& Def)
{
    if (ExpectedNext == NAME_None || SkillID != ExpectedNext)
    {
        BreakCombo();
        return;
    }

    ChainCount++;
    CurrentSkill = SkillID;
    ExpectedNext = Def.ComboTo;
    RemainingWindow = ComboWindow;
    OnComboSuccess.Broadcast(ChainCount);

    if (AActor* OwnerActor = GetOwner())
    {
        if (UBuffComponent* Buffs = OwnerActor->FindComponentByClass<UBuffComponent>())
        {
            const bool bGiveDamage = FMath::RandBool();
            const FName BuffID = bGiveDamage ? FName("ComboFlow") : FName("QiFocus");
            Buffs->ApplyBuffByID(BuffID, 1, 5.f);
        }
    }

    OnComboProgress.Broadcast(CurrentSkill, ExpectedNext, RemainingWindow);
}

void UComboComponent::BreakCombo()
{
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
