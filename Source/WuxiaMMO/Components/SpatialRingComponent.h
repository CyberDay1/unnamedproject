#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemInstance.h"
#include "Components/SpatialRingComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class USpatialRingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USpatialRingComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TArray<FItemInstance> StoredItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    int32 BaseSlots = 20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    int32 SlotsPerSubLevel = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    int32 CurrentMaxSlots = 20;

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void UpdateSlotCount(int32 SubLevel);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool AddItem(const FItemInstance& Item);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool RemoveItemByID(const FGuid& InstanceID, int32 Quantity);

    UFUNCTION(BlueprintPure, Category="Inventory")
    int32 GetFreeSlots() const;

private:
    bool MergeStackableItem(const FItemInstance& Item);
};
