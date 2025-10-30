#pragma once

#include "CoreMinimal.h"
#include "EClassAndElementEnums.generated.h"

UENUM(BlueprintType)
enum class EClassType : uint8
{
    None        UMETA(DisplayName = "None"),
    Warrior     UMETA(DisplayName = "Warrior"),
    Mystic      UMETA(DisplayName = "Mystic"),
    Ranger      UMETA(DisplayName = "Ranger"),
    Rogue       UMETA(DisplayName = "Rogue")
};

UENUM(BlueprintType)
enum class EElementType : uint8
{
    None    UMETA(DisplayName = "None"),
    Wood    UMETA(DisplayName = "Wood"),
    Fire    UMETA(DisplayName = "Fire"),
    Earth   UMETA(DisplayName = "Earth"),
    Metal   UMETA(DisplayName = "Metal"),
    Water   UMETA(DisplayName = "Water")
};
