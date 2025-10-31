#include "Combat/DamageCalculation.h"

float UDamageCalculation::ComputeDamage(const FSkillRow& Skill, float AttackerScalingStat, float ElementAffinityBonus, float GlobalAtkMult, float TargetDefense)
{
    const float ScaleContribution = Skill.StatMultiplier * AttackerScalingStat;
    const float ElementContribution = ElementModifier(Skill.Element) * (1.0f + ElementAffinityBonus);
    const float RawDamage = (Skill.BaseDamage + ScaleContribution) * ElementContribution * FMath::Max(0.1f, GlobalAtkMult);
    const float Mitigated = FMath::Max(1.f, RawDamage - TargetDefense);
    return Mitigated;
}

float UDamageCalculation::ElementModifier(EElementType Elem)
{
    switch (Elem)
    {
        case EElementType::Metal:
        case EElementType::Wood:
        case EElementType::Water:
        case EElementType::Fire:
        case EElementType::Earth:
            return 1.05f;
        default:
            return 1.0f;
    }
}
