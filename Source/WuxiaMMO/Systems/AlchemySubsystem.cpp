#include "Systems/AlchemySubsystem.h"
#include "Math/UnrealMathUtility.h"

static float Clamp01(float V) { return FMath::Clamp(V, 0.0f, 1.0f); }

float UAlchemySubsystem::QualityTierMultiplier(EItemQuality Q)
{
    switch (Q)
    {
        case EItemQuality::Poor:      return 0.5f;
        case EItemQuality::Mediocre:  return 0.75f;
        case EItemQuality::Normal:    return 1.0f;
        case EItemQuality::Greater:   return 1.25f;
        case EItemQuality::Rare:      return 1.5f;
        case EItemQuality::Legendary: return 2.0f;
        case EItemQuality::Immortal:  return 3.0f;
        default:                      return 1.0f;
    }
}

float UAlchemySubsystem::ApplySuccessChance(int32 AlchemyLevel, const FPillRecipe& Recipe, const TArray<FAlchemyModifier>& Modifiers, const FCauldronData& Cauldron) const
{
    // Prototype curve:
    // Base + (AlchemyLevel * 0.005) - (Tier * 0.02) + Cauldron + Modifiers
    float Chance = Recipe.BaseSuccessChance
                 + (static_cast<float>(AlchemyLevel) * 0.005f)
                 - (static_cast<int32>(Recipe.Tier) * 0.02f)
                 + Cauldron.BonusSuccessPct;

    for (const FAlchemyModifier& M : Modifiers)
    {
        if (M.Type == EAlchemyModifierType::SuccessBoost)
            Chance += M.Magnitude;
        // FailureProtect handled at consumption site; not part of chance directly
    }
    return Clamp01(Chance);
}

float UAlchemySubsystem::RollQualityMultiplier(float Base, const TArray<FAlchemyModifier>& Modifiers, const FCauldronData& Cauldron) const
{
    // Randomize around base then apply boosts
    float Roll = Base * FMath::FRandRange(0.9f, 1.1f);
    Roll += Cauldron.BonusQualityPct;

    for (const FAlchemyModifier& M : Modifiers)
    {
        if (M.Type == EAlchemyModifierType::QualityBoost)
            Roll += M.Magnitude;
        else if (M.Type == EAlchemyModifierType::EfficiencyBoost)
        {
            // EfficiencyBoost will be applied to pill decay elsewhere; no direct quality impact here
        }
    }
    return FMath::Max(0.0f, Roll);
}

void UAlchemySubsystem::CraftPill(int32 AlchemyLevel, const FPillRecipe& Recipe, const TArray<FAlchemyModifier>& ActiveModifiers,
                                  FCauldronData& Cauldron, bool& bSuccess, EItemQuality& OutQuality, float& DurabilityDelta)
{
    // Compute chances
    const float SuccessChance = ApplySuccessChance(AlchemyLevel, Recipe, ActiveModifiers, Cauldron);
    const float RandVal = FMath::FRand();

    // Base durability losses
    const float SuccessLoss = -1.0f;
    const float FailureLoss = -3.0f - (Recipe.Tier - EPillTier::Tier1) * 1.0f; // -3..-7 across tiers
    const float CriticalLoss = -10.0f;

    // Failure protect?
    bool bHasFailureProtect = false;
    for (const FAlchemyModifier& M : ActiveModifiers)
    {
        if (M.Type == EAlchemyModifierType::FailureProtect && M.Uses > 0)
        {
            bHasFailureProtect = true;
            break;
        }
    }

    if (RandVal <= SuccessChance)
    {
        bSuccess = true;
        DurabilityDelta = SuccessLoss;

        // Compute quality multiplier and map to tier
        const float Mult = RollQualityMultiplier(Recipe.BaseQualityMultiplier, ActiveModifiers, Cauldron);

        // Map rough multiplier to quality tiers
        if (Mult >= 2.5f)       OutQuality = EItemQuality::Immortal;
        else if (Mult >= 1.8f)  OutQuality = EItemQuality::Legendary;
        else if (Mult >= 1.35f) OutQuality = EItemQuality::Rare;
        else if (Mult >= 1.15f) OutQuality = EItemQuality::Greater;
        else if (Mult >= 0.9f)  OutQuality = EItemQuality::Normal;
        else if (Mult >= 0.65f) OutQuality = EItemQuality::Mediocre;
        else                    OutQuality = EItemQuality::Poor;
    }
    else
    {
        // Failure: decide if critical
        const bool bCritical = (FMath::FRand() < 0.15f); // 15% proto critical fail
        if (bCritical && !bHasFailureProtect)
        {
            bSuccess = false;
            OutQuality = EItemQuality::Poor;
            DurabilityDelta = CriticalLoss;
        }
        else
        {
            bSuccess = false;
            OutQuality = EItemQuality::Poor;
            DurabilityDelta = FailureLoss;
        }
    }

    // Apply durability
    Cauldron.Durability = FMath::Clamp(Cauldron.Durability + DurabilityDelta, 0.0f, Cauldron.MaxDurability);
    // When 0, future systems can impose penalties (e.g., +50% failure chance); stub only here.
}
