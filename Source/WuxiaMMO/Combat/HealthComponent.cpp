#include "Combat/HealthComponent.h"

#include "GameFramework/Actor.h"

void UHealthComponent::RecalcMaxHP()
{
    const float MinorRealmScale = 2.0f * FMath::Max(0, MinorRealmLevel);
    MaxHP = FMath::Max(1.f, BaseHP + ClassHPBonus + VitalityContribution + MinorRealmScale + ConstitutionMinorBonus);
    if (CurrentHP > MaxHP)
    {
        CurrentHP = MaxHP;
    }
}

void UHealthComponent::InitializeHP()
{
    RecalcMaxHP();
    if (CurrentHP <= 0.f)
    {
        CurrentHP = MaxHP;
    }
    bIsDead = false;
    OnHealthChanged.Broadcast(CurrentHP, MaxHP, nullptr);
}

void UHealthComponent::ApplyDamage(float Amount, AActor* InstigatorActor)
{
    if (bIsDead || Amount <= 0.f)
    {
        return;
    }

    CurrentHP = FMath::Clamp(CurrentHP - Amount, 0.f, MaxHP);
    OnHealthChanged.Broadcast(CurrentHP, MaxHP, InstigatorActor);

    if (CurrentHP <= 0.f)
    {
        bIsDead = true;
        OnDeath.Broadcast(InstigatorActor);
    }
}

void UHealthComponent::Heal(float Amount, AActor* InstigatorActor)
{
    if (bIsDead || Amount <= 0.f)
    {
        return;
    }

    CurrentHP = FMath::Clamp(CurrentHP + Amount, 0.f, MaxHP);
    OnHealthChanged.Broadcast(CurrentHP, MaxHP, InstigatorActor);
}

void UHealthComponent::ForceDeath(AActor* Killer)
{
    if (bIsDead)
    {
        return;
    }

    CurrentHP = 0.f;
    bIsDead = true;
    OnDeath.Broadcast(Killer);
}

void UHealthComponent::RespawnAtLocation(const FVector& WorldLocation)
{
    if (AActor* Owner = GetOwner())
    {
        Owner->SetActorLocation(WorldLocation, false, nullptr, ETeleportType::TeleportPhysics);
        bIsDead = false;
        InitializeHP();
    }
}
