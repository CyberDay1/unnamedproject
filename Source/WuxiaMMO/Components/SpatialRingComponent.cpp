#include "Components/SpatialRingComponent.h"

USpatialRingComponent::USpatialRingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentMaxSlots = BaseSlots;
}

void USpatialRingComponent::UpdateSlotCount(int32 SubLevel)
{
    CurrentMaxSlots = BaseSlots + (SubLevel * SlotsPerSubLevel);
    StoredItems.SetNum(FMath::Min(StoredItems.Num(), CurrentMaxSlots));
}

bool USpatialRingComponent::AddItem(const FItemInstance& Item)
{
    if (MergeStackableItem(Item))
        return true;

    if (StoredItems.Num() >= CurrentMaxSlots)
        return false;

    StoredItems.Add(Item);
    return true;
}

bool USpatialRingComponent::MergeStackableItem(const FItemInstance& Item)
{
    for (FItemInstance& Existing : StoredItems)
    {
        if (Existing.IsStackableWith(Item) && Existing.Quantity < Existing.Definition.MaxStack)
        {
            const int32 Space = Existing.Definition.MaxStack - Existing.Quantity;
            const int32 ToAdd = FMath::Min(Space, Item.Quantity);
            Existing.Quantity += ToAdd;

            if (ToAdd < Item.Quantity)
            {
                return false;
            }
            return true;
        }
    }
    return false;
}

bool USpatialRingComponent::RemoveItemByID(const FGuid& InstanceID, int32 Quantity)
{
    for (int32 i = 0; i < StoredItems.Num(); ++i)
    {
        if (StoredItems[i].InstanceID == InstanceID)
        {
            if (StoredItems[i].Quantity <= Quantity)
                StoredItems.RemoveAt(i);
            else
                StoredItems[i].Quantity -= Quantity;
            return true;
        }
    }
    return false;
}

int32 USpatialRingComponent::GetFreeSlots() const
{
    return CurrentMaxSlots - StoredItems.Num();
}
