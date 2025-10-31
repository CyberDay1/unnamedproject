#include "Environment/WeatherSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    GetWorld()->GetTimerManager().SetTimer(TickHandle, this, &UWeatherSubsystem::Tick, 1.0f, true);
}

void UWeatherSubsystem::Tick()
{
    const float Now = GetWorld()->GetTimeSeconds();
    for (int32 i = ActiveWeather.Num()-1; i >= 0; --i)
    {
        FActiveWeather& W = ActiveWeather[i];
        if (W.Duration > 0.f && (Now - W.StartTime) >= W.Duration)
        {
            const FName RID = W.Region.RegionID;
            const EWeatherType WT = W.Type;
            ActiveWeather.RemoveAt(i);
            OnWeatherChanged.Broadcast(RID, WT);
        }
    }
}

void UWeatherSubsystem::SetWeather(const FEnvironmentRegion& Region, EWeatherType Type, float DurationSeconds)
{
    const float Now = GetWorld()->GetTimeSeconds();
    // Replace existing entry for same RegionID & Type
    for (FActiveWeather& W : ActiveWeather)
    {
        if (W.Region.RegionID == Region.RegionID && W.Type == Type)
        {
            W.StartTime = Now;
            W.Duration  = DurationSeconds;
            W.Region    = Region;
            OnWeatherChanged.Broadcast(Region.RegionID, Type);
            return;
        }
    }
    FActiveWeather NewW; NewW.Region = Region; NewW.Type = Type; NewW.StartTime = Now; NewW.Duration = DurationSeconds;
    ActiveWeather.Add(NewW);
    OnWeatherChanged.Broadcast(Region.RegionID, Type);
}

void UWeatherSubsystem::ClearWeather(FName RegionID, EWeatherType Type)
{
    for (int32 i = ActiveWeather.Num()-1; i >= 0; --i)
    {
        if (ActiveWeather[i].Region.RegionID == RegionID && ActiveWeather[i].Type == Type)
        {
            ActiveWeather.RemoveAt(i);
            OnWeatherChanged.Broadcast(RegionID, Type);
        }
    }
}

void UWeatherSubsystem::TriggerBloodMoon(float DurationSeconds)
{
    FEnvironmentRegion Global; Global.RegionID = TEXT("Global"); Global.Radius = 0.f;
    SetWeather(Global, EWeatherType::BloodMoon, DurationSeconds);
}

void UWeatherSubsystem::TriggerBlueMoon(float DurationSeconds)
{
    FEnvironmentRegion Global; Global.RegionID = TEXT("Global"); Global.Radius = 0.f;
    SetWeather(Global, EWeatherType::BlueMoon, DurationSeconds);
}

void UWeatherSubsystem::TriggerQiFogRain(const FEnvironmentRegion& Region, float DurationSeconds)
{
    SetWeather(Region, EWeatherType::QiFogRain, DurationSeconds);
}

FEnvironmentModifiers UWeatherSubsystem::ModifiersFor(EWeatherType Type)
{
    FEnvironmentModifiers M; // defaults 1.0
    switch (Type)
    {
        case EWeatherType::QiFogRain:
            M.QiRegenMult    = 1.5f;  // +50%
            M.MeditationMult = 1.25f; // +25%
            break;
        case EWeatherType::BlueMoon:
            M.GatheringYieldMult = 1.25f;
            break;
        case EWeatherType::BloodMoon:
            M.SpawnRateMult   = 5.0f;  // 5x spawns (non-boss)
            M.MonsterAtkMult  = 1.25f; // +25% ATK
            M.MonsterDefMult  = 1.25f; // +25% DEF
            M.LeashRadiusMult = 2.0f;  // roam further
            break;
        default:
            break; // standard weather = visuals only
    }
    return M;
}

bool UWeatherSubsystem::IsInRegion(const FEnvironmentRegion& R, const FVector& P)
{
    if (R.Radius <= 0.f) return true; // global
    return FVector::DistSquared(P, R.Center) <= FMath::Square(R.Radius);
}

FEnvironmentModifiers UWeatherSubsystem::GetModifiersAtLocation(const FVector& WorldPos) const
{
    FEnvironmentModifiers Acc;
    const float Now = GetWorld()->GetTimeSeconds();
    for (const FActiveWeather& W : ActiveWeather)
    {
        const bool bExpired = (W.Duration > 0.f) && ((Now - W.StartTime) >= W.Duration);
        if (bExpired) continue;
        if (IsInRegion(W.Region, WorldPos))
        {
            Acc = FEnvironmentModifiers::Combine(Acc, ModifiersFor(W.Type));
        }
    }
    return Acc;
}

FEnvironmentModifiers UWeatherSubsystem::GetModifiersForRegion(FName RegionID) const
{
    FEnvironmentModifiers Acc;
    const float Now = GetWorld()->GetTimeSeconds();
    for (const FActiveWeather& W : ActiveWeather)
    {
        const bool bExpired = (W.Duration > 0.f) && ((Now - W.StartTime) >= W.Duration);
        if (bExpired) continue;
        if (W.Region.RegionID == RegionID || W.Region.Radius <= 0.f) // region or global
        {
            Acc = FEnvironmentModifiers::Combine(Acc, ModifiersFor(W.Type));
        }
    }
    return Acc;
}
