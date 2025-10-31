#include "Combat/HealthComponent.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UHealthComponent::ApplyDamage(float Amount, AActor* InstigatorActor)
{
    if (bIsDead)
    {
        return;
    }

    const float ClampedAmount = FMath::Max(0.f, Amount);
    HP = FMath::Clamp(HP - ClampedAmount, 0.f, MaxHP);

    OnDamaged.Broadcast(InstigatorActor, ClampedAmount);

    if (HP <= 0.f)
    {
        HandleDeath(InstigatorActor);
    }
}

void UHealthComponent::Heal(float Amount)
{
    if (bIsDead)
    {
        return;
    }

    const float ClampedAmount = FMath::Max(0.f, Amount);
    HP = FMath::Clamp(HP + ClampedAmount, 0.f, MaxHP);
}

void UHealthComponent::HandleDeath(AActor* Killer)
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    OnDeath.Broadcast(Killer);

    if (AActor* Owner = GetOwner())
    {
        Owner->SetActorEnableCollision(false);

        TInlineComponentArray<UActorComponent*> Components;
        Owner->GetComponents(Components);
        for (UActorComponent* Component : Components)
        {
            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                Primitive->SetSimulatePhysics(false);
            }
        }
    }

    StartFadeAndDestroy();
}

void UHealthComponent::StartFadeAndDestroy()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FTimerHandle TimerHandle;
    World->GetTimerManager().SetTimer(
        TimerHandle,
        [WeakThis = TWeakObjectPtr<UHealthComponent>(this)]()
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            if (AActor* Owner = WeakThis->GetOwner())
            {
                Owner->Destroy();
            }
        },
        5.0f,
        false);
}
