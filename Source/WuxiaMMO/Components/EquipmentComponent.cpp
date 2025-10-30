#include "Components/EquipmentComponent.h"

UEquipmentComponent::UEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UEquipmentComponent::EquipItem(EEquipmentSlot Slot, const FItemInstance& Item)
{
    if (EquippedItems.Contains(Slot))
        return false;
    EquippedItems.Add(Slot, Item);
    return true;
}

bool UEquipmentComponent::UnequipItem(EEquipmentSlot Slot, FItemInstance& OutItem)
{
    if (!EquippedItems.Contains(Slot))
        return false;

    OutItem = EquippedItems[Slot];
    EquippedItems.Remove(Slot);
    return true;
}

bool UEquipmentComponent::IsSlotOccupied(EEquipmentSlot Slot) const
{
    return EquippedItems.Contains(Slot);
}
