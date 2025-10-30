#include "Components/SpatialRingComponent.h"
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
    {
        SaveTriggerFromOwner(this, TEXT("InventoryChange:AddStack"));
        return true;
    }

    if (StoredItems.Num() >= CurrentMaxSlots)
        return false;

    StoredItems.Add(Item);
    SaveTriggerFromOwner(this, TEXT("InventoryChange:AddSlot"));
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
            SaveTriggerFromOwner(this, TEXT("InventoryChange:Remove"));
            return true;
        }
    }
    return false;
}

int32 USpatialRingComponent::GetFreeSlots() const
{
    return CurrentMaxSlots - StoredItems.Num();
}
