#include "Components/WuxiaReplicationComponent.h"

UWuxiaReplicationComponent::UWuxiaReplicationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UWuxiaReplicationComponent::InitializeReplication()
{
}
