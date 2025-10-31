#include "Components/SkillComponent.h"

#include "Buffs/BuffComponent.h"
#include "Components/ComboComponent.h"
#include "Kismet/KismetMathLibrary.h"

USkillComponent::USkillComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool USkillComponent::CanUseSkill(const FActiveSkillData& Skill, float CurrentQi, float ActualQiCost) const
{
    return (Skill.CooldownRemaining <= 0.0f) && (CurrentQi >= ActualQiCost);
}

void USkillComponent::ApplyCooldown(FActiveSkillData& Skill)
{
    Skill.CooldownRemaining = Skill.Definition.Cooldown;
}

float USkillComponent::CalculateDamage(const FSkillDefinition& Skill, const FClassBalanceData& ClassData, const TMap<FString, float>& Stats) const
{
    const float A = Stats.Contains("Agility") ? Stats["Agility"] : 0.0f;
    const float V = Stats.Contains("Vitality") ? Stats["Vitality"] : 0.0f;
    const float W = Stats.Contains("Wisdom") ? Stats["Wisdom"] : 0.0f;
    const float S = Stats.Contains("Spirit") ? Stats["Spirit"] : 0.0f;
    const float P = Stats.Contains("Willpower") ? Stats["Willpower"] : 0.0f;

    const float Scale = A*ClassData.AgilityScale + V*ClassData.VitalityScale +
                        W*ClassData.WisdomScale + S*ClassData.SpiritScale + P*ClassData.WillpowerScale;

    return Skill.BasePower * (1.0f + (Scale * 0.01f));
}

bool USkillComponent::UseSkill(FName SkillID, float CurrentQi, float& OutQiCost, float& OutDamage)
{
    if (!KnownSkills.Contains(SkillID))
    {
        return false;
    }

    FActiveSkillData& Skill = KnownSkills[SkillID];
    const FClassBalanceData ClassData = (ClassBalanceTable.Num() > 0) ? ClassBalanceTable[0] : FClassBalanceData();

    UBuffComponent* BuffComponent = GetOwner() ? GetOwner()->FindComponentByClass<UBuffComponent>() : nullptr;
    if (BuffComponent && BuffComponent->IsCastingBlocked())
    {
        return false;
    }

    float QiCostMultiplier = ClassData.QiEfficiency;
    if (bLastInputWasKeyCombo && Skill.Definition.bKeyComboEnabled)
    {
        QiCostMultiplier *= 0.9f;
    }

    if (BuffComponent)
    {
        QiCostMultiplier *= BuffComponent->GetQiCostMultiplier();
    }

    OutQiCost = Skill.Definition.QiCost * QiCostMultiplier;

    if (!CanUseSkill(Skill, CurrentQi, OutQiCost))
    {
        return false;
    }

    TMap<FString, float> Stats;
    Stats.Add("Agility", 5.0f);
    Stats.Add("Vitality", 5.0f);
    Stats.Add("Wisdom", 5.0f);
    Stats.Add("Spirit", 5.0f);
    Stats.Add("Willpower", 5.0f);

    OutDamage = CalculateDamage(Skill.Definition, ClassData, Stats);

    if (BuffComponent)
    {
        OutDamage *= BuffComponent->GetDamageMultiplier();
    }

    ApplyCooldown(Skill);

    if (AActor* OwnerActor = GetOwner())
    {
        if (UComboComponent* ComboComponent = OwnerActor->FindComponentByClass<UComboComponent>())
        {
            if (ComboComponent->IsInCombo())
            {
                if (ComboComponent->GetExpectedNext() == SkillID)
                {
                    ComboComponent->TryContinueCombo(SkillID, Skill.Definition.ComboTo);
                }
                else
                {
                    ComboComponent->BreakCombo();
                    if (Skill.Definition.ComboFrom.IsNone())
                    {
                        ComboComponent->StartCombo(SkillID, Skill.Definition.ComboTo);
                    }
                }
            }
            else if (Skill.Definition.ComboFrom.IsNone())
            {
                ComboComponent->StartCombo(SkillID, Skill.Definition.ComboTo);
            }
        }
    }

    bLastInputWasKeyCombo = false;

    OnSkillUsed.Broadcast(SkillID);
    return true;
}

void USkillComponent::AddSkillXP(FName SkillID, float Amount, ECultivationStage PlayerRealm)
{
    if (!KnownSkills.Contains(SkillID))
    {
        return;
    }

    FActiveSkillData& Skill = KnownSkills[SkillID];
    Skill.CurrentXP += Amount;
    const float Threshold = Skill.Definition.BaseSkillXP * FMath::Pow(2.0f, Skill.CurrentLevel - 1);

    if (Skill.CurrentXP >= Threshold && Skill.CurrentLevel < Skill.Definition.MaxLevelPerRealm)
    {
        if (static_cast<uint8>(PlayerRealm) >= static_cast<uint8>(Skill.Definition.RequiredRealm))
        {
            Skill.CurrentXP = 0.0f;
            Skill.CurrentLevel++;
            OnSkillLevelUp.Broadcast(SkillID, Skill.CurrentLevel);
        }
    }
}

void USkillComponent::TickCooldowns(float DeltaSeconds)
{
    for (auto& Pair : KnownSkills)
    {
        if (Pair.Value.CooldownRemaining > 0.0f)
        {
            Pair.Value.CooldownRemaining = FMath::Max(0.0f, Pair.Value.CooldownRemaining - DeltaSeconds);
        }
    }
}
