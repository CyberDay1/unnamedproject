#include "Spawning/SpawnPoint.h"
#include "Spawning/SpawnSubsystem.h"

void ASpawnPoint::Register()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (USpawnSubsystem* SS = GI->GetSubsystem<USpawnSubsystem>())
        {
            SS->RegisterGroup(Group);
        }
    }
}

void ASpawnPoint::BeginPlay()
{
    Super::BeginPlay();
    if (Group.Region.Center.IsNearlyZero()) Group.Region.Center = GetActorLocation();
    Register();
}
