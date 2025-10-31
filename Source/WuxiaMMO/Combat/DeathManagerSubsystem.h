#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DeathManagerSubsystem.generated.h"

class AActor;

UCLASS()
class UDeathManagerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Death")
    void HandleDeath(AActor* Victim, AActor* Killer, bool bIsPvP);

    UFUNCTION(BlueprintCallable, Category="Death")
    void SetRespawnPoints(AActor* Player, const FVector& SectLoc, const FVector& CityLoc);

    UFUNCTION(BlueprintPure, Category="Death")
    bool GetRespawnPoints(AActor* Player, FVector& OutSectLoc, FVector& OutCityLoc) const;

private:
    TMap<TWeakObjectPtr<AActor>, TPair<FVector, FVector>> RespawnMap;
};
