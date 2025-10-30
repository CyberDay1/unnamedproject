#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EFaction.h"
#include "FSectReputation.h"
#include "ReputationComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UReputationComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UReputationComponent();

    // Faction standings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reputation")
    TMap<EFaction, float> FactionReputation; // -10000..+10000

    // Sect standings (supports player-created sects too)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reputation")
    TArray<FSectReputation> SectReps;

    UFUNCTION(BlueprintCallable, Category="Reputation")
    void AddFactionReputation(EFaction Faction, float Amount);

    UFUNCTION(BlueprintCallable, Category="Reputation")
    float GetFactionReputation(EFaction Faction) const;

    UFUNCTION(BlueprintCallable, Category="Reputation")
    void AddSectReputation(FName SectID, float Amount);

    UFUNCTION(BlueprintCallable, Category="Reputation")
    float GetSectReputation(FName SectID) const;

private:
    FSectReputation* FindSect(FName SectID);
};
