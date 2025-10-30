#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Items/ItemInstance.h"
#include "InventorySubsystem.generated.h"

UCLASS()
class UInventorySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Inventory|Persistence")
    void SaveInventory(const FString& CharacterID, const TArray<FItemInstance>& Items);

    UFUNCTION(BlueprintCallable, Category="Inventory|Persistence")
    bool LoadInventory(const FString& CharacterID, TArray<FItemInstance>& OutItems) const;
};
