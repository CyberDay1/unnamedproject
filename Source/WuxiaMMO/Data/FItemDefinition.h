#pragma once
#include "CoreMinimal.h"
#include "FItemDefinition.generated.h"

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
    Consumable,
    Material,
    Resource,
    Food,
    Equipment,
    Quest
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    Poor,
    Mediocre,
    Normal,
    Greater,
    Rare,
    Legendary,
    Immortal
};

USTRUCT(BlueprintType)
struct FItemDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    EItemCategory Category = EItemCategory::Consumable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    EItemRarity Rarity = EItemRarity::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    int32 MaxStack = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    float Weight = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FString Description;
};
