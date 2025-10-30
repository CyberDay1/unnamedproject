#pragma once
#include "CoreMinimal.h"
#include "EFaction.generated.h"

// Keep names aligned with world powers; extend later for dynamic factions if needed.
UENUM(BlueprintType)
enum class EFaction : uint8
{
    Orthodox,
    DemonicCultivators,
    HeavenlyDemonSect,
    BloodCultist,
    Neutral
};
