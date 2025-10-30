#pragma once
#include "CoreMinimal.h"
#include "FEquipmentSlot.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    Headpiece,
    Chest,
    Leggings,
    Feet,
    Gloves,
    Ring1,
    Ring2,
    Ring3,
    Ring4,
    Belt,
    Necklace,
    SpiritCape,
    Cauldron
};
