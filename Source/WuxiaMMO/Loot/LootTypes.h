#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LootTypes.generated.h"

UENUM(BlueprintType)
enum class ELootRarity : uint8
{
    Trash,
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary
};

USTRUCT(BlueprintType)
struct FLootItem
{
    GENERATED_BODY()

    // Item to grant (your item system’s definition/row/ID)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID = NAME_None;

    // Fixed quantity range
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinQty = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxQty = 1;

    // Base drop chance in [0..1], before luck/weather mods (e.g., 0.10 = 10%)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseChance = 0.f;

    // Rarity used for routing (trash vs non-trash) and auction logic
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ELootRarity Rarity = ELootRarity::Common;

    // Relative weight for “pick one of” groups (optional)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight = 1.f;
};

USTRUCT(BlueprintType)
struct FLootGroup // one row in a DataTable; monsters reference a GroupID
{
    GENERATED_BODY()

    // RowName = DropTableID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    // Items always rolled independently (each can drop)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FLootItem> Independent;

    // Choose N from Weighted list (optional)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FLootItem> Weighted;

    // How many items to pick from Weighted (after weighting)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PickFromWeighted = 0;

    // Blue Moon multiplier affects quantities of ALL successful drops
    // Blood Moon adds +10% rarity weighting when evaluating Weighted rarities
};
