#include "Combat/EffectTickerSubsystem.h"

#include "Combat/DamageResolverSubsystem.h"
#include "Engine/World.h"
#include "Stats/Stats.h"

int32 UEffectTickerSubsystem::AddTickEffect(const FDamageSpec& Spec, float IntervalSeconds, float DurationSeconds)
{
    FPendingTickEffect Effect;
    Effect.Spec = Spec;
    Effect.Interval = FMath::Max(0.05f, IntervalSeconds);
    Effect.TimeLeft = FMath::Max(0.05f, DurationSeconds);
    Effect.Accum = 0.f;

    const int32 Handle = NextHandle++;
    Active.Add(Handle, Effect);
    return Handle;
}

void UEffectTickerSubsystem::RemoveTickEffect(int32 Handle)
{
    Active.Remove(Handle);
}

void UEffectTickerSubsystem::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (Active.Num() == 0)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    UDamageResolverSubsystem* Resolver = World->GetSubsystem<UDamageResolverSubsystem>();
    if (!Resolver)
    {
        return;
    }

    TArray<int32> Completed;

    for (TPair<int32, FPendingTickEffect>& Pair : Active)
    {
        FPendingTickEffect& Effect = Pair.Value;
        Effect.TimeLeft -= DeltaSeconds;
        Effect.Accum += DeltaSeconds;

        while (Effect.Accum >= Effect.Interval && Effect.TimeLeft > 0.f)
        {
            Effect.Accum -= Effect.Interval;
            Resolver->ResolveAndApplyDamage(Effect.Spec);
        }

        if (Effect.TimeLeft <= 0.f)
        {
            Completed.Add(Pair.Key);
        }
    }

    for (int32 Handle : Completed)
    {
        Active.Remove(Handle);
    }
}

TStatId UEffectTickerSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UEffectTickerSubsystem, STATGROUP_Tickables);
}
