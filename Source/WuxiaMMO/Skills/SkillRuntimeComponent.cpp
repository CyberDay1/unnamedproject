#include "Skills/SkillRuntimeComponent.h"
#include "Skills/SkillRegistrySubsystem.h"
#include "Skills/SkillMasteryComponent.h"
#include "Skills/ComboComponent.h"
#include "Buffs/BuffComponent.h"
#include "Combat/CombatComponent.h"
#include "Combat/DamageCalculation.h"
#include "Components/CultivationComponent.h"
#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h"
#include "Engine/World.h"

namespace
{
    constexpr float MasteryXPPerUse = 0.12f;
}

double USkillRuntimeComponent::Now() const
{
    return GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
}

bool USkillRuntimeComponent::GetDef(FName SkillID, FSkillDefinition& Out) const
{
    if (RuntimeSkillTableOverride)
    {
        if (const FSkillDefinition* Row = RuntimeSkillTableOverride->FindRow<FSkillDefinition>(SkillID, TEXT("Skills")))
        {
            Out = *Row;
            return true;
        }
    }

    if (const UWorld* World = GetWorld())
    {
        if (const UGameInstance* GameInstance = World->GetGameInstance())
        {
            if (const USkillRegistrySubsystem* Registry = GameInstance->GetSubsystem<USkillRegistrySubsystem>())
            {
                if (Registry->SkillTable)
                {
                    if (const FSkillDefinition* Row = Registry->SkillTable->FindRow<FSkillDefinition>(SkillID, TEXT("Skills")))
                    {
                        Out = *Row;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool USkillRuntimeComponent::PassesUnlocks(const FSkillDefinition& Def) const
{
    const AActor* OwnerActor = GetOwner();

    if (!Def.RequiresTags.IsEmpty())
    {
        FGameplayTagContainer OwnedTags;
        if (const IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(OwnerActor))
        {
            TagInterface->GetOwnedGameplayTags(OwnedTags);
        }

        if (!OwnedTags.HasAll(Def.RequiresTags))
        {
            return false;
        }
    }

    int32 MajorRealmStep = 0;
    if (const UCultivationComponent* Cultivation = OwnerActor ? OwnerActor->FindComponentByClass<UCultivationComponent>() : nullptr)
    {
        MajorRealmStep = static_cast<int32>(Cultivation->Progress.Stage) + 1;

        if (Def.UnlockMajorRealmStep > 0 && MajorRealmStep < Def.UnlockMajorRealmStep)
        {
            return false;
        }
    }
    else if (Def.UnlockMajorRealmStep > 0)
    {
        return false;
    }

    if (Def.Kind == ESkillKind::Ultimate && MajorRealmStep > 0 && (MajorRealmStep % 3) != 0)
    {
        return false;
    }

    return true;
}

bool USkillRuntimeComponent::SpendQi(const FSkillDefinition& Def)
{
    if (Def.QiCostFraction <= 0.f)
    {
        return true;
    }

    float CostFraction = Def.QiCostFraction;

    if (bLastInputWasKeyCombo && Def.bKeyComboEnabled)
    {
        CostFraction *= 0.9f;
    }

    if (AActor* OwnerActor = GetOwner())
    {
        if (UBuffComponent* Buffs = OwnerActor->FindComponentByClass<UBuffComponent>())
        {
            CostFraction *= Buffs->GetQiCostMultiplier();
        }

        if (UCultivationComponent* Cultivation = OwnerActor->FindComponentByClass<UCultivationComponent>())
        {
            const float MaxQi = FMath::Max(Cultivation->Progress.RequiredQi, 1.f);
            const float QiCost = MaxQi * CostFraction;

            if (Cultivation->Progress.CurrentQi < QiCost)
            {
                return false;
            }

            Cultivation->Progress.CurrentQi -= QiCost;
        }
    }

    bLastInputWasKeyCombo = false;

    return true;
}

void USkillRuntimeComponent::RegisterInputSource(bool bIsKeyCombo)
{
    bLastInputWasKeyCombo = bIsKeyCombo;
}

bool USkillRuntimeComponent::LearnSkill(FName SkillID)
{
    KnownSkills.Add(SkillID);
    return true;
}

bool USkillRuntimeComponent::IsOnCooldown(FName SkillID) const
{
    if (const double* ExpireTime = CooldownUntil.Find(SkillID))
    {
        return Now() < *ExpireTime;
    }

    return false;
}

void USkillRuntimeComponent::ForceCooldown(FName SkillID, float Seconds)
{
    CooldownUntil.Add(SkillID, Now() + Seconds);
}

void USkillRuntimeComponent::GetKnownSkills(TArray<FName>& OutSkillIDs) const
{
    OutSkillIDs.Reset(KnownSkills.Num());
    for (const FName& Skill : KnownSkills)
    {
        OutSkillIDs.Add(Skill);
    }
}

float USkillRuntimeComponent::GetCooldownRemaining(FName SkillID) const
{
    if (const double* ExpireTime = CooldownUntil.Find(SkillID))
    {
        return FMath::Max(0.f, static_cast<float>(*ExpireTime - Now()));
    }

    return 0.f;
}

bool USkillRuntimeComponent::UseSkillByID(FName SkillID)
{
    if (!KnownSkills.Contains(SkillID))
    {
        return false;
    }

    if (IsOnCooldown(SkillID))
    {
        return false;
    }

    FSkillDefinition Definition;
    if (!GetDef(SkillID, Definition))
    {
        return false;
    }

    if (!PassesUnlocks(Definition))
    {
        return false;
    }

    AActor* OwnerActor = GetOwner();
    UBuffComponent* Buffs = OwnerActor ? OwnerActor->FindComponentByClass<UBuffComponent>() : nullptr;

    if (Buffs && Buffs->IsCastingBlocked())
    {
        return false;
    }

    if (!SpendQi(Definition))
    {
        return false;
    }

    OnSkillCast.Broadcast(SkillID, OwnerActor, Definition);

    USkillMasteryComponent* MasteryComponent = OwnerActor ? OwnerActor->FindComponentByClass<USkillMasteryComponent>() : nullptr;
    const float CooldownMultiplier = MasteryComponent ? MasteryComponent->GetCooldownMultiplier(SkillID, Definition) : 1.f;

    if (Definition.Kind != ESkillKind::Passive)
    {
        if (UCombatComponent* Combat = OwnerActor ? OwnerActor->FindComponentByClass<UCombatComponent>() : nullptr)
        {
            bool bAnyHit = false;

            const int32 HitCount = FMath::Max(1, Definition.NumHits);
            for (int32 Index = 0; Index < HitCount; ++Index)
            {
                if (!Combat->PerformSkillByID(SkillID, nullptr))
                {
                    bAnyHit = bAnyHit || Combat->PerformBasicAttack(nullptr);
                }
                else
                {
                    bAnyHit = true;
                }

                OnSkillImpact.Broadcast(SkillID, nullptr, Definition);
            }

            if (bAnyHit)
            {
                if (UComboComponent* Combo = OwnerActor ? OwnerActor->FindComponentByClass<UComboComponent>() : nullptr)
                {
                    if (Combo->IsInCombo())
                    {
                        Combo->TryContinueCombo(SkillID, Definition);
                    }
                    else if (Definition.ComboFrom == NAME_None)
                    {
                        Combo->StartCombo(SkillID, Definition);
                    }
                }
            }
        }
    }

    ForceCooldown(SkillID, Definition.Kind == ESkillKind::Ultimate ? Definition.UltimateCooldownSeconds * CooldownMultiplier : Definition.CooldownSeconds * CooldownMultiplier);
    OnSkillEnd.Broadcast(SkillID, Definition);

    if (MasteryComponent && Definition.Kind != ESkillKind::Passive)
    {
        MasteryComponent->GrantXP(SkillID, MasteryXPPerUse);
    }

    return true;
}

