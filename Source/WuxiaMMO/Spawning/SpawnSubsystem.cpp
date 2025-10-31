#include "Spawning/SpawnSubsystem.h"
#include "Environment/WeatherSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

void USpawnSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    GetWorld()->GetTimerManager().SetTimer(TickHandle, this, &USpawnSubsystem::Tick, 2.0f, true);
}

void USpawnSubsystem::Deinitialize()
{
    if (UWorld* W = GetWorld()) W->GetTimerManager().ClearTimer(TickHandle);
    Super::Deinitialize();
}

void USpawnSubsystem::RegisterGroup(const FSpawnGroup& Group)
{
    Groups.Add(Group);
}

void USpawnSubsystem::ClearGroups()
{
    Groups.Empty();
    Live.Empty();
}

int32 USpawnSubsystem::GetLiveCount(FName GroupID) const
{
    if (const TArray<TWeakObjectPtr<APawn>>* Arr = Live.Find(GroupID))
        return Arr->Num();
    return 0;
}

void USpawnSubsystem::CullDead()
{
    for (auto& Pair : Live)
    {
        TArray<TWeakObjectPtr<APawn>>& A = Pair.Value;
        A.RemoveAll([](const TWeakObjectPtr<APawn>& P){ return !P.IsValid(); });
    }
}

float USpawnSubsystem::BloodMoonSpawnMultiplier() const
{
    if (const UGameInstance* GI = GetGameInstance())
    {
        if (const UWeatherSubsystem* WS = GI->GetSubsystem<UWeatherSubsystem>())
        {
            const FEnvironmentModifiers Global = WS->GetModifiersForRegion(TEXT("Global"));
            return FMath::Max(1.f, Global.SpawnRateMult);
        }
    }
    return 1.f;
}

APawn* USpawnSubsystem::SpawnOne(const FSpawnGroup& G)
{
    if (!G.Entries.Num()) return nullptr;

    // Weighted pick respecting tiers (bosses/elite can be included)
    float Sum = 0.f;
    for (const auto& E : G.Entries) Sum += E.Weight;
    float R = FMath::FRand() * FMath::Max(0.01f, Sum);
    const FSpawnEntry* Chosen = nullptr;
    for (const auto& E : G.Entries) { if ((R -= E.Weight) <= 0.f) { Chosen = &E; break; } }
    if (!Chosen || !Chosen->PawnClass) return nullptr;

    const FVector Loc = (G.Region.Radius > 0.f)
        ? (G.Region.Center + FMath::VRand() * FMath::FRandRange(0.f, G.Region.Radius))
        : G.Region.Center;
    const FRotator Rot(0, FMath::FRandRange(0.f, 360.f), 0);

    FActorSpawnParameters SP; SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    APawn* P = GetWorld()->SpawnActor<APawn>(Chosen->PawnClass, Loc, Rot, SP);

    if (P)
    {
        TArray<TWeakObjectPtr<APawn>>& Arr = Live.FindOrAdd(G.GroupID);
        Arr.Add(P);
    }
    return P;
}

void USpawnSubsystem::Tick()
{
    CullDead();

    const float BloodMult = BloodMoonSpawnMultiplier();
    for (FSpawnGroup& G : Groups)
    {
        // Population cap scaled by Blood Moon (5x spawns implied via faster fill)
        const int32 Cap = FMath::CeilToInt(G.PopulationCap * BloodMult);
        const int32 Current = GetLiveCount(G.GroupID);

        if (Current < Cap)
        {
            // Spawn pacing: compute how many to try this tick
            const int32 Need = Cap - Current;
            const int32 ToSpawn = FMath::Clamp(Need, 1, FMath::CeilToInt(Need * 0.5f));
            for (int32 i=0; i<ToSpawn; ++i) SpawnOne(G);
        }

        // Near-player rares
        if (G.bEnableNearPlayerRares && FMath::FRand() < G.RareChance * (BloodMult >= 5.f ? 2.f : 1.f))
        {
            // Try spawn a rare (e.g., Treasure Pig) near a random player
            TArray<AActor*> Players; UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), Players);
            if (Players.Num() > 0)
            {
                const AActor* Anchor = Players[FMath::RandRange(0, Players.Num()-1)];
                FSpawnEntry RarePick; bool bFound=false;
                for (const auto& E : G.Entries) if (E.Tier == ESpawnTier::Rare) { RarePick = E; bFound=true; break; }
                if (bFound && RarePick.PawnClass)
                {
                    const FVector Loc = Anchor->GetActorLocation() + FMath::VRand() * 1500.f;
                    FActorSpawnParameters SP; SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                    GetWorld()->SpawnActor<APawn>(RarePick.PawnClass, Loc, FRotator::ZeroRotator, SP);
                }
            }
        }
    }
}
