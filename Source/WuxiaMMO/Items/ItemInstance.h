#pragma once
#include "CoreMinimal.h"
#include "FItemDefinition.h"
#include "ItemInstance.generated.h"

USTRUCT(BlueprintType)
struct FItemInstance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FItemDefinition Definition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FGuid InstanceID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    int32 Quantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    float Durability = 100.0f;

    FItemInstance()
    {
        InstanceID = FGuid::NewGuid();
    }

    bool IsStackableWith(const FItemInstance& Other) const
    {
        return Definition.ItemID == Other.Definition.ItemID &&
               Definition.MaxStack > 1 &&
               Definition.Category != EItemCategory::Equipment;
    }
};
