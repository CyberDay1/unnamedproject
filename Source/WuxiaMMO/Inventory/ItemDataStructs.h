#pragma once

#include "CoreMinimal.h"
#include "ItemDataStructs.generated.h"

USTRUCT(BlueprintType)
struct FWuxiaItemData
{
    GENERATED_BODY()

    FWuxiaItemData()
        : ItemID(NAME_None)
        , DisplayName(FText::GetEmpty())
        , MaxStackSize(1)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    int32 MaxStackSize;
};
