#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WuxiaReplicationComponent.generated.h"

UCLASS(ClassGroup=(Networking), meta=(BlueprintSpawnableComponent))
class UWuxiaReplicationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWuxiaReplicationComponent();

    UFUNCTION(BlueprintCallable, Category="Replication")
    void InitializeReplication();
};
