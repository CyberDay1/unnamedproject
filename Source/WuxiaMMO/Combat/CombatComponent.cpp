#include "Combat/CombatComponent.h"

#include "Combat/DamageCalculation.h"
#include "Combat/HealthComponent.h"
#include "Components/CultivationComponent.h"
#include "Environment/WeatherSubsystem.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

#include <cfloat>

namespace
{
static double GetWorldTimeSeconds(const UWorld* World)
{
    return World ? World->GetTimeSeconds() : 0.0;
}
}

bool UCombatComponent::PerformBasicAttack(AActor* PreferredTarget)
{
    if (!GetOwner())
    {
        return false;
    }

    AActor* Target = PreferredTarget;
    if (!Target)
    {
        const FVector Origin = GetOwner()->GetActorLocation();
        if (!SphereTraceForTarget(Origin, BasicRadius, BasicRange, Target))
        {
            return false;
        }
    }

    if (!Target || !CanDamageTarget(Target, false))
    {
        return false;
    }

    FSkillRow TempSkill;
    TempSkill.BaseDamage = BasicAttackBase;
    TempSkill.StatMultiplier = BasicAttackMult;
    TempSkill.Element = EElementType::None;
    TempSkill.Range = BasicRange;
    TempSkill.Radius = BasicRadius;

    const float Damage = UDamageCalculation::ComputeDamage(
        TempSkill,
        GetAttackerScalingStat(),
        GetElementAffinityBonus(TempSkill.Element),
        ComputeGlobalAttackMultiplier(!GetOwner()->IsPlayerControlled()),
        GetTargetDefense(Target));

    if (UHealthComponent* Health = Target->FindComponentByClass<UHealthComponent>())
    {
        Health->ApplyDamage(Damage, GetOwner());
        OnHitTarget.Broadcast(Target, Damage, false, NAME_None);
        return true;
    }

    return false;
}

bool UCombatComponent::PerformSkillByID(FName SkillID, AActor* PreferredTarget)
{
    if (!GetOwner())
    {
        return false;
    }

    FSkillRow SkillRow;
    if (!GetSkillRow(SkillID, SkillRow))
    {
        return false;
    }

    if (!IsSkillReady(SkillID))
    {
        return false;
    }

    AActor* Target = PreferredTarget;
    if (!Target)
    {
        const FVector Origin = GetOwner()->GetActorLocation();
        if (!SphereTraceForTarget(Origin, SkillRow.Radius, SkillRow.Range, Target))
        {
            return false;
        }
    }

    if (!Target || !CanDamageTarget(Target, true))
    {
        return false;
    }

    const float Damage = UDamageCalculation::ComputeDamage(
        SkillRow,
        GetAttackerScalingStat(),
        GetElementAffinityBonus(SkillRow.Element),
        ComputeGlobalAttackMultiplier(!GetOwner()->IsPlayerControlled()),
        GetTargetDefense(Target));

    if (UHealthComponent* Health = Target->FindComponentByClass<UHealthComponent>())
    {
        Health->ApplyDamage(Damage, GetOwner());
        OnHitTarget.Broadcast(Target, Damage, true, SkillID);
        SetCooldown(SkillID, SkillRow.CooldownSeconds);
        return true;
    }

    return false;
}

bool UCombatComponent::IsSkillReady(FName SkillID) const
{
    const double* CooldownEnd = CooldownMap.Find(SkillID);
    if (!CooldownEnd)
    {
        return true;
    }

    return GetWorldTimeSeconds(GetWorld()) >= *CooldownEnd;
}

void UCombatComponent::SetCooldown(FName SkillID, float Seconds)
{
    CooldownMap.Add(SkillID, GetWorldTimeSeconds(GetWorld()) + Seconds);
}

bool UCombatComponent::GetSkillRow(FName SkillID, FSkillRow& OutRow) const
{
    if (!SkillDataTable)
    {
        return false;
    }

    if (const FSkillRow* Row = SkillDataTable->FindRow<FSkillRow>(SkillID, TEXT("CombatComponent")))
    {
        OutRow = *Row;
        return true;
    }

    return false;
}

bool UCombatComponent::SphereTraceForTarget(const FVector& Center, float Radius, float Range, AActor*& OutTarget) const
{
    OutTarget = nullptr;

    if (!GetOwner())
    {
        return false;
    }

    const FVector End = Center + GetOwner()->GetActorForwardVector() * Range;

    TArray<FHitResult> Hits;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetOwner());

    const bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
        this,
        Center,
        End,
        Radius,
        ObjectTypes,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        Hits,
        true);

    if (!bHit)
    {
        return false;
    }

    float BestDistanceSquared = FLT_MAX;
    for (const FHitResult& Hit : Hits)
    {
        if (AActor* HitActor = Hit.GetActor())
        {
            const float DistanceSquared = FVector::DistSquared(Center, HitActor->GetActorLocation());
            if (DistanceSquared < BestDistanceSquared)
            {
                BestDistanceSquared = DistanceSquared;
                OutTarget = HitActor;
            }
        }
    }

    return OutTarget != nullptr;
}

bool UCombatComponent::CanDamageTarget(AActor* Target, bool /*bIsSkill*/) const
{
    if (!Target || Target == GetOwner())
    {
        return false;
    }

    if (bInSafeZone && Target->IsPlayerControlled())
    {
        return false;
    }

    const bool bAttackerIsPlayer = GetOwner() && GetOwner()->IsPlayerControlled();
    const bool bTargetIsPlayer = Target->IsPlayerControlled();

    if (bAttackerIsPlayer && bTargetIsPlayer)
    {
        if (!bAllowPvP)
        {
            return false;
        }

        if (bDuelOnlyWithConsent)
        {
            // Designers can enforce mutual duel consent via gameplay tags or external checks.
        }
    }

    return true;
}

float UCombatComponent::ComputeGlobalAttackMultiplier(bool bIsMonster) const
{
    float Multiplier = 1.f;

    if (!bIsMonster)
    {
        return Multiplier;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return Multiplier;
    }

    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance)
    {
        return Multiplier;
    }

    if (const UWeatherSubsystem* WeatherSubsystem = GameInstance->GetSubsystem<UWeatherSubsystem>())
    {
        const FEnvironmentModifiers Modifiers = WeatherSubsystem->GetModifiersForRegion(TEXT("Global"));
        Multiplier *= Modifiers.MonsterAtkMult;
    }

    return Multiplier;
}

float UCombatComponent::GetAttackerScalingStat() const
{
    if (const AActor* OwnerActor = GetOwner())
    {
        if (const UCultivationComponent* Cultivation = OwnerActor->FindComponentByClass<UCultivationComponent>())
        {
            return Cultivation->Progress.CurrentQi;
        }
    }

    return 0.f;
}

float UCombatComponent::GetTargetDefense(const AActor* /*Target*/) const
{
    return 0.f;
}

float UCombatComponent::GetElementAffinityBonus(EElementType /*Elem*/) const
{
    return 0.f;
}
