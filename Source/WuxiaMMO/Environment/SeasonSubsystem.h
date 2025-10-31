#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EnvEnums.h"
#include "SeasonSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSeasonChanged, ESeason, Season);

UCLASS()
class USeasonSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // Real-time days per season
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Season") int32 RealDaysPerSeason = 7;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Season") ESeason CurrentSeason = ESeason::Spring;

    UPROPERTY(BlueprintAssignable) FOnSeasonChanged OnSeasonChanged;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintPure, Category="Season") ESeason GetCurrentSeason() const { return CurrentSeason; }
    UFUNCTION(BlueprintCallable, Category="Season") void ForceSetSeason(ESeason NewSeason);

private:
    FTimerHandle TickHandle;
    void Tick();
};
