#include "AI/ThreatComponent.h"

#include "GameFramework/Actor.h"

UThreatComponent::UThreatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UThreatComponent::BeginPlay()
{
    Super::BeginPlay();

    if (HomeLocation.IsZero())
    {
        if (AActor* Owner = GetOwner())
        {
            HomeLocation = Owner->GetActorLocation();
        }
    }
}

void UThreatComponent::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

    DecayThreat(DeltaSeconds);

    if (AActor* Owner = GetOwner())
    {
        const float DistanceFromHome = FVector::Dist(Owner->GetActorLocation(), HomeLocation);
        if (DistanceFromHome > PatrolRadius)
        {
            TimeOutsidePatrol += DeltaSeconds;
            if (TimeOutsidePatrol >= LeashResetSeconds)
            {
                ResetThreat();
                // TODO: Notify AI controller to retreat to HomeLocation.
            }
        }
        else
        {
            TimeOutsidePatrol = 0.f;
        }
    }
}

void UThreatComponent::AddDamageThreat(AActor* Instigator, float DamageAmount, bool bInstigatorIsTank)
{
    if (!Instigator || DamageAmount <= 0.f)
    {
        return;
    }

    const float Multiplier = bInstigatorIsTank ? TankThreatMultiplier : 1.f;
    const float ThreatAmount = BaseThreatPerDamage * DamageAmount * Multiplier;
    AccumulateThreat(Instigator, ThreatAmount);
}

void UThreatComponent::AddHealThreat(AActor* Healer, float HealAmount)
{
    if (!Healer || HealAmount <= 0.f)
    {
        return;
    }

    const float ThreatAmount = BaseThreatPerDamage * HealAmount * HealerThreatMultiplier;
    AccumulateThreat(Healer, ThreatAmount);
}

void UThreatComponent::DecayThreat(float DeltaSeconds)
{
    if (Entries.Num() == 0)
    {
        return;
    }

    const float DecayAmount = ThreatDecayPerSecond * DeltaSeconds;
    for (FThreatEntry& Entry : Entries)
    {
        Entry.Value = FMath::Max(0.f, Entry.Value - DecayAmount);
    }

    Entries.RemoveAll([](const FThreatEntry& Entry)
    {
        return Entry.Value <= 0.f || !Entry.Actor.IsValid();
    });
}

void UThreatComponent::ResetThreat()
{
    Entries.Reset();
    TimeOutsidePatrol = 0.f;
}

AActor* UThreatComponent::GetTopThreatActor() const
{
    float BestThreat = -1.f;
    AActor* BestActor = nullptr;
    for (const FThreatEntry& Entry : Entries)
    {
        if (AActor* Actor = Entry.Actor.Get())
        {
            if (Entry.Value > BestThreat)
            {
                BestThreat = Entry.Value;
                BestActor = Actor;
            }
        }
    }
    return BestActor;
}

void UThreatComponent::AccumulateThreat(AActor* SourceActor, float Amount)
{
    if (Amount <= 0.f)
    {
        return;
    }

    for (FThreatEntry& Entry : Entries)
    {
        if (Entry.Actor == SourceActor)
        {
            Entry.Value += Amount;
            return;
        }
    }

    FThreatEntry& NewEntry = Entries.AddDefaulted_GetRef();
    NewEntry.Actor = SourceActor;
    NewEntry.Value = Amount;
}
