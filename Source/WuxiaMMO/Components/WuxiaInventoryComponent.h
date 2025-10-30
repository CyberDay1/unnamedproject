#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WuxiaInventoryComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UWuxiaInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWuxiaInventoryComponent();

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void InitializeInventory();
};
