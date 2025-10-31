#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceNodeComponent.generated.h"

class AActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UResourceNodeComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UResourceNodeComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource") FName NodeID = TEXT("SpiritGrass");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource") float GatherTime = 3.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource") float CooldownTime = 120.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource") bool bInstancedPerPlayer = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource") FName RewardItemID = TEXT("Herb_SpiritGrass");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource") int32 RewardCount = 1;

    UFUNCTION(BlueprintCallable, Category="Resource") bool StartGather(AActor* Player);
    UFUNCTION(BlueprintPure, Category="Resource") bool IsAvailableFor(AActor* Player) const;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    struct FPerPlayerState
    {
        bool bOnCooldown = false;
        float TimeLeft = 0.f;
        bool bGathering = false;
        float GatherLeft = 0.f;
        TWeakObjectPtr<AActor> ActiveGatherer;
    };

    FPerPlayerState* GetState(AActor* Player);
    const FPerPlayerState* GetStateForRead(AActor* Player) const;
    void UpdateState(FPerPlayerState& State, float DeltaSeconds);
    void GrantReward(AActor* Player);

    TMap<TWeakObjectPtr<AActor>, FPerPlayerState> PerPlayerStates;
    FPerPlayerState SharedState;
    bool bHasSharedState = false;
};
