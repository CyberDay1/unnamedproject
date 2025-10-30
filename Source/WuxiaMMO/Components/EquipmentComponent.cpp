#include "Components/EquipmentComponent.h"
#include "GameFramework/Actor.h"
#include "Systems/ActionSaveRouterComponent.h"

static void SaveTriggerFromOwner(UActorComponent* Component, const TCHAR* Reason)
{
    if (!Component)
    {
        return;
    }

    if (AActor* Owner = Component->GetOwner())
    {
        if (UActionSaveRouterComponent* Router = Owner->FindComponentByClass<UActionSaveRouterComponent>())
        {
            Router->TriggerSave(Reason);
        }
    }
}

UEquipmentComponent::UEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UEquipmentComponent::EquipItem(EEquipmentSlot Slot, const FItemInstance& Item)
{
    if (EquippedItems.Contains(Slot))
        return false;
    EquippedItems.Add(Slot, Item);
    SaveTriggerFromOwner(this, TEXT("Equipment:Equip"));
    return true;
}

bool UEquipmentComponent::UnequipItem(EEquipmentSlot Slot, FItemInstance& OutItem)
{
    if (!EquippedItems.Contains(Slot))
        return false;

    OutItem = EquippedItems[Slot];
    EquippedItems.Remove(Slot);
    SaveTriggerFromOwner(this, TEXT("Equipment:Unequip"));
    return true;
}

bool UEquipmentComponent::IsSlotOccupied(EEquipmentSlot Slot) const
{
    return EquippedItems.Contains(Slot);
}
