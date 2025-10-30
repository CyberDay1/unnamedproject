#pragma once

#include "CoreMinimal.h"
#include "ECurrencyType.generated.h"

// Ordered ascending by value for conversion chains.
UENUM(BlueprintType)
enum class ECurrencyType : uint8
{
    SilverTael = 0,
    GoldTael,

    SmallSpiritStone_Poor,
    SmallSpiritStone_Average,
    SmallSpiritStone_Rich,

    LargeSpiritStone_Poor,
    LargeSpiritStone_Average,
    LargeSpiritStone_Rich,

    ImmortalStone_Poor,
    ImmortalStone_Average,
    ImmortalStone_Rich,
    ImmortalStone_Overflowing,

    SectContribution
};
