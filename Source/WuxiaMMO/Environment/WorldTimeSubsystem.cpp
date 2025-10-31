#include "Environment/WorldTimeSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UWorldTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    GetWorld()->GetTimerManager().SetTimer(TickHandle, this, &UWorldTimeSubsystem::Tick, 0.25f, true);
}

void UWorldTimeSubsystem::Deinitialize()
{
    if (UWorld* W = GetWorld())
        W->GetTimerManager().ClearTimer(TickHandle);
    Super::Deinitialize();
}

void UWorldTimeSubsystem::Tick()
{
    // Convert real time delta to game hours respecting day/night durations
    static double Last = FPlatformTime::Seconds();
    const double Now = FPlatformTime::Seconds();
    const float RealDelta = float(Now - Last);
    Last = Now;
    AdvanceByRealSeconds(RealDelta);
}

void UWorldTimeSubsystem::AdvanceByRealSeconds(float RealSeconds)
{
    const bool bIsDay = (CurrentPhase == EDayPhase::Day);
    const float PhaseLenSec = bIsDay ? RealSecondsPerDayPhase_Day : RealSecondsPerDayPhase_Night;
    const float GameHoursPerPhase = bIsDay ? 16.f : 8.f; // Day covers 16 hours (e.g., 06:00-22:00), Night covers 8 hours
    const float HoursDelta = (RealSeconds / FMath::Max(1.f, PhaseLenSec)) * GameHoursPerPhase;

    HourOfDay += HoursDelta;
    if (HourOfDay >= 24.f) HourOfDay -= 24.f;

    RecomputePhase();
    OnTimeUpdated.Broadcast(HourOfDay, CurrentPhase);
}

void UWorldTimeSubsystem::RecomputePhase()
{
    // Day ~ [06:00, 22:00), Night ~ [22:00, 06:00)
    const bool bDay = (HourOfDay >= 6.f && HourOfDay < 22.f);
    CurrentPhase = bDay ? EDayPhase::Day : EDayPhase::Night;
}
