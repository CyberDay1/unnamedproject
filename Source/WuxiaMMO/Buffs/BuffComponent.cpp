#include "Buffs/BuffComponent.h"

#include "Engine/DataTable.h"

UBuffComponent::UBuffComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UBuffComponent::BeginPlay()
{
    Super::BeginPlay();
    RecalculateCachedValues();
}

bool UBuffComponent::FindBuffDef(FName BuffID, FBuffDefinition& OutDefinition) const
{
    if (!BuffTable)
    {
        return false;
    }

    if (const FBuffDefinition* Row = BuffTable->FindRow<FBuffDefinition>(BuffID, TEXT("BuffComponent")))
    {
        OutDefinition = *Row;
        return true;
    }

    return false;
}

bool UBuffComponent::ApplyBuffByID(FName BuffID, int32 Stacks, float OverrideDurationSeconds)
{
    FBuffDefinition Definition;
    if (!FindBuffDef(BuffID, Definition))
    {
        return false;
    }

    const float NewDuration = OverrideDurationSeconds > 0.f ? OverrideDurationSeconds : Definition.DurationSeconds;

    for (FActiveBuff& Active : ActiveBuffs)
    {
        if (Active.BuffTag == Definition.BuffTag)
        {
            if (Definition.Application == EBuffApplication::Add)
            {
                Active.Stacks = FMath::Clamp(Active.Stacks + Stacks, 1, Definition.MaxStacks);
                Active.TimeRemaining = NewDuration;
            }
            else
            {
                Active.Stacks = 1;
                Active.TimeRemaining = FMath::Max(Active.TimeRemaining, NewDuration);
                Active.DamageMultiplier = FMath::Max(Active.DamageMultiplier, Definition.DamageMultiplier);
                Active.QiCostMultiplier = FMath::Min(Active.QiCostMultiplier, Definition.QiCostMultiplier);
                Active.DefenseMultiplier = FMath::Min(Active.DefenseMultiplier, Definition.DefenseMultiplier);
                Active.MoveSpeedMultiplier = FMath::Max(Active.MoveSpeedMultiplier, Definition.MoveSpeedMultiplier);
                Active.bBlocksCasting = Active.bBlocksCasting || Definition.bBlocksCasting;
                OnBuffApplied.Broadcast(Active, Active.Stacks);
                RecalculateCachedValues();
                return true;
            }

            Active.DamageMultiplier = Definition.DamageMultiplier;
            Active.QiCostMultiplier = Definition.QiCostMultiplier;
            Active.DefenseMultiplier = Definition.DefenseMultiplier;
            Active.MoveSpeedMultiplier = Definition.MoveSpeedMultiplier;
            Active.bBlocksCasting = Definition.bBlocksCasting;
            OnBuffApplied.Broadcast(Active, Active.Stacks);
            RecalculateCachedValues();
            return true;
        }
    }

    FActiveBuff NewBuff;
    NewBuff.BuffID = BuffID;
    NewBuff.BuffTag = Definition.BuffTag;
    NewBuff.Stacks = FMath::Clamp(Stacks, 1, Definition.MaxStacks);
    NewBuff.TimeRemaining = NewDuration;
    NewBuff.DamageMultiplier = Definition.DamageMultiplier;
    NewBuff.QiCostMultiplier = Definition.QiCostMultiplier;
    NewBuff.DefenseMultiplier = Definition.DefenseMultiplier;
    NewBuff.MoveSpeedMultiplier = Definition.MoveSpeedMultiplier;
    NewBuff.bBlocksCasting = Definition.bBlocksCasting;

    ActiveBuffs.Add(NewBuff);
    OnBuffApplied.Broadcast(NewBuff, NewBuff.Stacks);
    RecalculateCachedValues();
    return true;
}

bool UBuffComponent::RemoveBuffByTag(FGameplayTag BuffTag)
{
    for (int32 Index = 0; Index < ActiveBuffs.Num(); ++Index)
    {
        if (ActiveBuffs[Index].BuffTag == BuffTag)
        {
            const FActiveBuff Removed = ActiveBuffs[Index];
            ActiveBuffs.RemoveAt(Index);
            OnBuffRemoved.Broadcast(Removed);
            RecalculateCachedValues();
            return true;
        }
    }

    return false;
}

void UBuffComponent::RecalculateCachedValues()
{
    CachedDamageMultiplier = 1.f;
    CachedQiCostMultiplier = 1.f;
    CachedDefenseMultiplier = 1.f;
    CachedMoveSpeedMultiplier = 1.f;
    bCachedBlocksCasting = false;

    for (const FActiveBuff& Buff : ActiveBuffs)
    {
        CachedDamageMultiplier *= Buff.DamageMultiplier;
        CachedQiCostMultiplier *= Buff.QiCostMultiplier;
        CachedDefenseMultiplier *= Buff.DefenseMultiplier;
        CachedMoveSpeedMultiplier *= Buff.MoveSpeedMultiplier;
        bCachedBlocksCasting = bCachedBlocksCasting || Buff.bBlocksCasting;
    }
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    bool bChanged = false;
    for (int32 Index = ActiveBuffs.Num() - 1; Index >= 0; --Index)
    {
        ActiveBuffs[Index].TimeRemaining -= DeltaTime;
        if (ActiveBuffs[Index].TimeRemaining <= 0.f)
        {
            const FActiveBuff Removed = ActiveBuffs[Index];
            ActiveBuffs.RemoveAt(Index);
            OnBuffRemoved.Broadcast(Removed);
            bChanged = true;
        }
    }

    if (bChanged)
    {
        RecalculateCachedValues();
    }
}
