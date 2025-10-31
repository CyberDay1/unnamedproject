#include "Combat/DamageResolverSubsystem.h"

#include "Combat/PvPFlags.h"
#include "Combat/StatInterfaces.h"
#include "Buffs/BuffComponent.h"
#include "Combat/HealthComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

namespace
{
static FORCEINLINE float Clamp01(float Value)
{
    return FMath::Clamp(Value, 0.f, 1.f);
}
}

float UDamageResolverSubsystem::ComputeArmorMitigation(float Armor) const
{
    const float K = 100.f;
    return 1.f / (1.f + FMath::Max(0.f, Armor) / K);
}

float UDamageResolverSubsystem::ComputeElementMitigation(float Resist01) const
{
    return 1.f - Clamp01(Resist01);
}

float UDamageResolverSubsystem::RollCrit(float CritChance, float CritDamageMul) const
{
    if (FMath::FRand() <= Clamp01(CritChance))
    {
        return FMath::Max(1.f, CritDamageMul);
    }
    return 1.f;
}

bool UDamageResolverSubsystem::RollDodge(float HitChance, float DodgeChance) const
{
    const float Effective = Clamp01(HitChance) * (1.f - Clamp01(DodgeChance));
    return FMath::FRand() > Effective;
}

bool UDamageResolverSubsystem::PassesPvPGate(const AActor* Source, const AActor* Target) const
{
    if (!Source || !Target)
    {
        return false;
    }

    const bool bSourceImplements = Source->GetClass()->ImplementsInterface(UPvPFlagProvider::StaticClass());
    const bool bTargetImplements = Target->GetClass()->ImplementsInterface(UPvPFlagProvider::StaticClass());

    if (bSourceImplements && bTargetImplements)
    {
        const IPvPFlagProvider* SourceFlags = Cast<IPvPFlagProvider>(Source);
        const IPvPFlagProvider* TargetFlags = Cast<IPvPFlagProvider>(Target);

        if (!SourceFlags || !TargetFlags)
        {
            return false;
        }

        if (!SourceFlags->IsPvPEnabled() || !TargetFlags->IsPvPEnabled())
        {
            return false;
        }

        if (SourceFlags->IsSameFactionAs(Target))
        {
            return false;
        }

        if (SourceFlags->IsProtectedRegion() || TargetFlags->IsProtectedRegion())
        {
            return false;
        }
    }

    return true;
}

float UDamageResolverSubsystem::GetElementResist01(const AActor* Target, EElementType Elem) const
{
    if (!Target || Elem == EElementType::None)
    {
        return 0.f;
    }

    if (const ICombatStatProvider* Stats = Cast<ICombatStatProvider>(Target))
    {
        return Clamp01(Stats->GetResist(Elem));
    }

    return 0.f;
}

float UDamageResolverSubsystem::GetArmorValue(const AActor* Target) const
{
    if (const ICombatStatProvider* Stats = Cast<ICombatStatProvider>(Target))
    {
        return FMath::Max(0.f, Stats->GetArmor());
    }

    return 0.f;
}

float UDamageResolverSubsystem::GetOffenseStat(const AActor* Source) const
{
    if (const ICombatStatProvider* Stats = Cast<ICombatStatProvider>(Source))
    {
        return FMath::Max(0.f, Stats->GetAttackPower());
    }

    return 100.f;
}

float UDamageResolverSubsystem::GetCritChance(const AActor* Source) const
{
    if (const ICombatStatProvider* Stats = Cast<ICombatStatProvider>(Source))
    {
        return Clamp01(Stats->GetCritChance());
    }

    return 0.1f;
}

float UDamageResolverSubsystem::GetCritDamageMul(const AActor* Source) const
{
    if (const ICombatStatProvider* Stats = Cast<ICombatStatProvider>(Source))
    {
        return FMath::Max(1.f, Stats->GetCritDamage());
    }

    return 1.5f;
}

float UDamageResolverSubsystem::GetHitChance(const AActor* Source) const
{
    if (const ICombatStatProvider* Stats = Cast<ICombatStatProvider>(Source))
    {
        return Clamp01(Stats->GetHitChance());
    }

    return 0.9f;
}

float UDamageResolverSubsystem::GetDodgeChance(const AActor* Target) const
{
    if (const ICombatStatProvider* Stats = Cast<ICombatStatProvider>(Target))
    {
        return Clamp01(Stats->GetDodge());
    }

    return 0.05f;
}

FDamageResult UDamageResolverSubsystem::ResolveAndApplyDamage(const FDamageSpec& Spec)
{
    FDamageResult Result;

    AActor* Source = Spec.Source.Get();
    AActor* Target = Spec.Target.Get();

    if (!Source || !Target)
    {
        return Result;
    }

    const bool bBothPlayers = Source->ActorHasTag(FName("Player")) && Target->ActorHasTag(FName("Player"));
    if (bBothPlayers && !PassesPvPGate(Source, Target))
    {
        Result.bBlockedByPvP = true;
        return Result;
    }

    float Damage = FMath::Max(0.f, Spec.BaseAmount);
    if (FMath::IsNearlyZero(Damage))
    {
        Damage = GetOffenseStat(Source);
    }

    if (UBuffComponent* SourceBuffs = Source->FindComponentByClass<UBuffComponent>())
    {
        Damage *= SourceBuffs->GetDamageMultiplier();
    }

    if (UBuffComponent* TargetBuffs = Target->FindComponentByClass<UBuffComponent>())
    {
        const float DefenseMultiplier = TargetBuffs->GetDefenseMultiplier();
        if (!FMath::IsNearlyZero(DefenseMultiplier))
        {
            Damage *= 1.f / DefenseMultiplier;
        }
    }

    switch (Spec.School)
    {
        case EDamageSchool::Physical:
        {
            const float Armor = GetArmorValue(Target);
            Damage *= ComputeArmorMitigation(Armor);
            break;
        }
        case EDamageSchool::Elemental:
        {
            const float Resist = GetElementResist01(Target, Spec.Element);
            Damage *= ComputeElementMitigation(Resist);
            break;
        }
        default:
            break;
    }

    if (Spec.bCanDodge)
    {
        if (RollDodge(GetHitChance(Source), GetDodgeChance(Target)))
        {
            Result.bDodged = true;
            Result.FinalAmount = 0.f;
            return Result;
        }
    }

    float CritMultiplier = 1.f;
    if (Spec.bCanCrit)
    {
        CritMultiplier = RollCrit(GetCritChance(Source), GetCritDamageMul(Source));
        Result.bCrit = CritMultiplier > 1.f;
    }

    Damage *= CritMultiplier;

    Result.FinalAmount = FMath::Max(0.f, Damage);

    if (UHealthComponent* Health = Target->FindComponentByClass<UHealthComponent>())
    {
        Health->ApplyDamage(Result.FinalAmount, Source);
    }

    return Result;
}
