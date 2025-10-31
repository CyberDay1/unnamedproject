#include "Gathering/ResourceNodeComponent.h"

#include "GameFramework/Actor.h"

UResourceNodeComponent::UResourceNodeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UResourceNodeComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UResourceNodeComponent::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

    if (bInstancedPerPlayer)
    {
        for (auto It = PerPlayerStates.CreateIterator(); It; ++It)
        {
            UpdateState(It.Value(), DeltaSeconds);

            if (!It.Key().IsValid() && !It.Value().bGathering && !It.Value().bOnCooldown)
            {
                It.RemoveCurrent();
            }
        }
    }
    else if (bHasSharedState)
    {
        UpdateState(SharedState, DeltaSeconds);
    }
}

bool UResourceNodeComponent::StartGather(AActor* Player)
{
    if (!Player)
    {
        return false;
    }

    FPerPlayerState* State = GetState(Player);
    if (!State || State->bOnCooldown || State->bGathering)
    {
        return false;
    }

    State->bGathering = true;
    State->GatherLeft = GatherTime;
    State->ActiveGatherer = Player;
    return true;
}

bool UResourceNodeComponent::IsAvailableFor(AActor* Player) const
{
    if (!bInstancedPerPlayer)
    {
        if (!bHasSharedState)
        {
            return true;
        }

        return !SharedState.bGathering && !SharedState.bOnCooldown;
    }

    if (!Player)
    {
        return false;
    }

    const TWeakObjectPtr<AActor> Key(Player);
    if (const FPerPlayerState* State = PerPlayerStates.Find(Key))
    {
        return !State->bGathering && !State->bOnCooldown;
    }

    return true;
}

UResourceNodeComponent::FPerPlayerState* UResourceNodeComponent::GetState(AActor* Player)
{
    if (bInstancedPerPlayer)
    {
        const TWeakObjectPtr<AActor> Key(Player);
        return &PerPlayerStates.FindOrAdd(Key);
    }

    bHasSharedState = true;
    return &SharedState;
}

const UResourceNodeComponent::FPerPlayerState* UResourceNodeComponent::GetStateForRead(AActor* Player) const
{
    if (bInstancedPerPlayer)
    {
        const TWeakObjectPtr<AActor> Key(Player);
        return PerPlayerStates.Find(Key);
    }

    return bHasSharedState ? &SharedState : nullptr;
}

void UResourceNodeComponent::UpdateState(FPerPlayerState& State, float DeltaSeconds)
{
    if (State.bGathering)
    {
        State.GatherLeft -= DeltaSeconds;
        if (State.GatherLeft <= 0.f)
        {
            State.bGathering = false;
            State.bOnCooldown = true;
            State.TimeLeft = CooldownTime;

            if (AActor* Gatherer = State.ActiveGatherer.Get())
            {
                GrantReward(Gatherer);
            }

            State.ActiveGatherer = nullptr;
        }
    }
    else if (State.bOnCooldown)
    {
        State.TimeLeft -= DeltaSeconds;
        if (State.TimeLeft <= 0.f)
        {
            State.bOnCooldown = false;
            State.TimeLeft = 0.f;
        }
    }
    else
    {
        State.ActiveGatherer = nullptr;
    }
}

void UResourceNodeComponent::GrantReward(AActor* Player)
{
    // TODO: Integrate with the inventory or loot system to grant the gathered resources.
    // Example:
    // if (UInventoryComponent* Inventory = Player->FindComponentByClass<UInventoryComponent>())
    // {
    //     Inventory->AddItem(RewardItemID, RewardCount);
    // }
}
