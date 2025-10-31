#pragma once

#include "CoreMinimal.h"
#include "Combat/DamageTypes.h"
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

