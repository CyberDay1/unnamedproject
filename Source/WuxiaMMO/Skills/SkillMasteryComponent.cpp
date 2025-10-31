#include "Skills/SkillMasteryComponent.h"

FSkillProgress* USkillMasteryComponent::FindOrAdd(FName SkillID)
{
    for (auto& Skill : Skills)
    {
        if (Skill.SkillID == SkillID)
        {
            return &Skill;
        }
    }

    FSkillProgress Progress;
    Progress.SkillID = SkillID;
    Skills.Add(Progress);
    return &Skills.Last();
}

void USkillMasteryComponent::GrantXP(FName SkillID, float Amount)
{
    FSkillProgress* Progress = FindOrAdd(SkillID);
    if (!Progress)
    {
        return;
    }

    if (Progress->MasteryLevel >= 10)
    {
        return;
    }

    Progress->MasteryXP += FMath::Max(0.f, Amount);

    while (Progress->MasteryXP >= 1.f && Progress->MasteryLevel < 10)
    {
        Progress->MasteryXP -= 1.f;
        Progress->MasteryLevel++;
        OnLevelUp.Broadcast(SkillID, Progress->MasteryLevel);
    }
}

int32 USkillMasteryComponent::GetLevel(FName SkillID) const
{
    for (const auto& Skill : Skills)
    {
        if (Skill.SkillID == SkillID)
        {
            return Skill.MasteryLevel;
        }
    }

    return 0;
}

float USkillMasteryComponent::GetLevelDamageBonus(FName SkillID, const FSkillDefinition& Def) const
{
    const int32 Level = GetLevel(SkillID);
    const float Step = FMath::Lerp(Def.Mastery.DamagePerLevelMin, Def.Mastery.DamagePerLevelMax, 0.5f);
    return 1.f + Step * Level;
}

float USkillMasteryComponent::GetQiCostMultiplier(FName SkillID, const FSkillDefinition& Def) const
{
    const int32 Level = GetLevel(SkillID);
    return Def.bReduceQiWithMastery ? FMath::Pow(1.f - Def.Mastery.QiCostRedPerLevel, Level) : 1.f;
}

float USkillMasteryComponent::GetCooldownMultiplier(FName SkillID, const FSkillDefinition& Def) const
{
    const int32 Level = GetLevel(SkillID);
    return Def.bReduceCooldownWithMastery ? FMath::Pow(1.f - Def.Mastery.CooldownRedPerLevel, Level) : 1.f;
}

