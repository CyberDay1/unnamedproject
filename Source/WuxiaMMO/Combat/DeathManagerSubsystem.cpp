#include "Combat/DeathManagerSubsystem.h"

#include "GameFramework/Actor.h"

void UDeathManagerSubsystem::HandleDeath(AActor* Victim, AActor* Killer, bool bIsPvP)
{
    if (!Victim)
    {
        return;
    }

    // Hook into cultivation progression to apply XP loss when available.
    // Example:
    // if (UCultivationComponent* Cultivation = Victim->FindComponentByClass<UCultivationComponent>())
    // {
    //     Cultivation->ApplyMinorRealmXPLoss(bIsPvP ? 0.05f : 0.25f);
    // }

    // Additional penalties (item loss, etc.) can be added here.
}

void UDeathManagerSubsystem::SetRespawnPoints(AActor* Player, const FVector& SectLoc, const FVector& CityLoc)
{
    if (!Player)
    {
        return;
    }

    RespawnMap.FindOrAdd(Player) = TPair<FVector, FVector>(SectLoc, CityLoc);
}

bool UDeathManagerSubsystem::GetRespawnPoints(AActor* Player, FVector& OutSectLoc, FVector& OutCityLoc) const
{
    if (!Player)
    {
        return false;
    }

    if (const TPair<FVector, FVector>* Found = RespawnMap.Find(Player))
    {
        OutSectLoc = Found->Key;
        OutCityLoc = Found->Value;
        return true;
    }

    return false;
}
