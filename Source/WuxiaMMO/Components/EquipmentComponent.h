#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/FEquipmentSlot.h"
#include "Items/ItemInstance.h"
#include "EquipmentComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEquipmentComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipment")
    TMap<EEquipmentSlot, FItemInstance> EquippedItems;

    UFUNCTION(BlueprintCallable, Category="Equipment")
    bool EquipItem(EEquipmentSlot Slot, const FItemInstance& Item);

    UFUNCTION(BlueprintCallable, Category="Equipment")
    bool UnequipItem(EEquipmentSlot Slot, FItemInstance& OutItem);

    UFUNCTION(BlueprintPure, Category="Equipment")
    bool IsSlotOccupied(EEquipmentSlot Slot) const;
};
