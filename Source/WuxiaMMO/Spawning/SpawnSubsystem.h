#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SpawnTypes.h"
#include "SpawnSubsystem.generated.h"

class UWeatherSubsystem;

UCLASS()
class USpawnSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning") TArray<FSpawnGroup> Groups;

    // Track live actors per group
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawning") TMap<FName, TArray<TWeakObjectPtr<APawn>>> Live;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Blueprint API
    UFUNCTION(BlueprintCallable, Category="Spawning") void RegisterGroup(const FSpawnGroup& Group);
    UFUNCTION(BlueprintCallable, Category="Spawning") void ClearGroups();
    UFUNCTION(BlueprintCallable, Category="Spawning") int32 GetLiveCount(FName GroupID) const;

private:
    FTimerHandle TickHandle;
    void Tick();

    APawn* SpawnOne(const FSpawnGroup& G);
    void CullDead();

    float BloodMoonSpawnMultiplier() const;
};
