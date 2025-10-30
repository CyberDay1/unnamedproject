#pragma once
#include "CoreMinimal.h"
#include "EClassAndElementEnums.generated.h"

UENUM(BlueprintType)
enum class EClassType : uint8
{
    SwordCultivator UMETA(DisplayName="Sword Cultivator"),
    ShadowDisciple UMETA(DisplayName="Shadow Disciple"),
    TalismanSage UMETA(DisplayName="Talisman Sage"),
    FivePhaseAdept UMETA(DisplayName="Five-Phase Adept"),
    IronBodyMonk UMETA(DisplayName="Iron Body Monk"),
    SpearSaint UMETA(DisplayName="Spear Saint"),
    SoundWeaver UMETA(DisplayName="Sound Weaver"),
    PillMaster UMETA(DisplayName="Pill Master"),
    BeastCultivator UMETA(DisplayName="Beast Cultivator"),
    ArraySage UMETA(DisplayName="Array Sage"),
    None UMETA(DisplayName="None")
};

UENUM(BlueprintType)
enum class EElementType : uint8
{
    Wood UMETA(DisplayName="Wood"),
    Fire UMETA(DisplayName="Fire"),
    Earth UMETA(DisplayName="Earth"),
    Metal UMETA(DisplayName="Metal"),
    Water UMETA(DisplayName="Water"),
    None UMETA(DisplayName="None")
};
