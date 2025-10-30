#include "Components/CultivationComponent.h"
#include "Components/SpatialRingComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

UCultivationComponent::UCultivationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    RecomputeRequirement();
}

float UCultivationComponent::StageIndex(ECultivationStage S) const
{
    // Map enum to ascending indices for curve
    return static_cast<float>(static_cast<uint8>(S));
}

void UCultivationComponent::RecomputeRequirement()
{
    // Aggressive curve: RequiredQi = BaseQi * StageMultiplier^(StageIdx) * SubLevelMultiplier^(SubLevel-1)
    const float StageIdx = StageIndex(Progress.Stage);
    Progress.RequiredQi = BaseQi * UKismetMathLibrary::Pow(StageMultiplier, StageIdx)
                                 * UKismetMathLibrary::Pow(SubLevelMultiplier, static_cast<float>(Progress.SubLevel - 1));

    // If at SubLevel 9 and already full, block additional Qi until breakthrough
    Progress.bQiGainBlockedAtCap = (Progress.SubLevel == 9) && (Progress.CurrentQi >= Progress.RequiredQi);
    UpdateSpatialRingSlots();
}

void UCultivationComponent::AddQiFlat(float Amount)
{
    if (Progress.bQiGainBlockedAtCap || Amount <= 0.0f) return;
    Progress.CurrentQi = FMath::Min(Progress.CurrentQi + Amount, Progress.RequiredQi);
    // Re-check cap only matters on sub-level 9
    if (Progress.SubLevel == 9 && Progress.CurrentQi >= Progress.RequiredQi)
        Progress.bQiGainBlockedAtCap = true;
}

void UCultivationComponent::AddQiPercentOfRequirement(float Percent)
{
    if (Percent <= 0.0f) return;
    AddQiFlat(Progress.RequiredQi * Percent);
}

void UCultivationComponent::ConsumePill(const FName& PillName, float BaseQiOrPercent, bool bIsPercent, float QualityMultiplier)
{
    if (Progress.bQiGainBlockedAtCap) return;

    // Efficiency: 1% less per same-name pill used
    int32& Count = Progress.PillEfficiencyCounters.FindOrAdd(PillName);
    const float Efficiency = FMath::Clamp(1.0f - (Count * 0.01f), 0.0f, 1.0f);
    Count++;

    const float BaseGain = bIsPercent ? (Progress.RequiredQi * BaseQiOrPercent) : BaseQiOrPercent;
    const float FinalGain = FMath::Max(0.0f, BaseGain * QualityMultiplier * Efficiency);
    AddQiFlat(FinalGain);
}

void UCultivationComponent::ApplyMeditationQi(float Amount)
{
    AddQiFlat(Amount);
}

void UCultivationComponent::TickPassiveQi(float DeltaSeconds)
{
    const float Gain = PassiveQiPerSec * FMath::Max(DeltaSeconds, 0.0f);
    AddQiFlat(Gain);
}

bool UCultivationComponent::TryBreakthrough(bool bHasStageCatalystItem)
{
    // Must be at cap and sub-level 9 to stage-advance; otherwise sub-level up if Qi full
    if (Progress.CurrentQi < Progress.RequiredQi) return false;

    if (Progress.SubLevel < 9)
    {
        // Advance sub-level: consume stored Qi and recompute
        Progress.SubLevel++;
        Progress.CurrentQi = 0.0f;
        Progress.bQiGainBlockedAtCap = false;
        RecomputeRequirement();
        UpdateSpatialRingSlots();
        return true;
    }

    // SubLevel == 9 → require catalyst item for stage breakthrough
    if (!bHasStageCatalystItem)
        return false;

    // Success path (prototype: guaranteed). Future: add chance/backlash.
    // Advance stage, reset sub-level to 1, clear stored Qi.
    Progress.Stage = static_cast<ECultivationStage>(FMath::Min<int32>(static_cast<int32>(Progress.Stage) + 1, static_cast<int32>(ECultivationStage::PrimordialDeity)));
    Progress.SubLevel = 1;
    Progress.CurrentQi = 0.0f;
    Progress.bQiGainBlockedAtCap = false;
    RecomputeRequirement();
    UpdateSpatialRingSlots();

    OnBreakthrough.Broadcast(Progress.Stage, Progress.SubLevel);
    return true;
}

void UCultivationComponent::SetSpatialRing(USpatialRingComponent* InSpatialRing)
{
    SpatialRing = InSpatialRing;
    UpdateSpatialRingSlots();
}

void UCultivationComponent::UpdateSpatialRingSlots() const
{
    if (SpatialRing.IsValid())
    {
        SpatialRing->UpdateSlotCount(Progress.SubLevel);
    }
}
