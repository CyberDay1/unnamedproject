#include "Environment/SeasonSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"

void USeasonSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // Poll daily-ish
    GetWorld()->GetTimerManager().SetTimer(TickHandle, this, &USeasonSubsystem::Tick, 3600.f, true);
    Tick();
}

void USeasonSubsystem::Tick()
{
    // Derive season from epoch weeks
    const FDateTime Now = FDateTime::UtcNow();
    const int64 Days = Now.GetTicks() / ETimespan::TicksPerDay;
    const int64 SeasonIndex = (RealDaysPerSeason > 0) ? (Days / RealDaysPerSeason) % 4 : 0;
    ESeason NewSeason = static_cast<ESeason>(SeasonIndex);
    if (NewSeason != CurrentSeason)
    {
        CurrentSeason = NewSeason;
        OnSeasonChanged.Broadcast(CurrentSeason);
    }
}

void USeasonSubsystem::ForceSetSeason(ESeason NewSeason)
{
    if (NewSeason != CurrentSeason)
    {
        CurrentSeason = NewSeason;
        OnSeasonChanged.Broadcast(CurrentSeason);
    }
}
