#pragma once
#include "CoreMinimal.h"
#include "ECultivationStage.generated.h"

UENUM(BlueprintType)
enum class ECultivationStage : uint8
{
    MortalBody UMETA(DisplayName="Mortal Body"),
    QiRefining UMETA(DisplayName="Qi Refining"),
    FoundationEstablishment UMETA(DisplayName="Foundation Establishment"),
    CoreFormation UMETA(DisplayName="Core Formation"),
    NascentSoul UMETA(DisplayName="Nascent Soul"),
    SoulTransformation UMETA(DisplayName="Soul Transformation"),
    SpiritSevering UMETA(DisplayName="Spirit Severing"),
    VoidRefinement UMETA(DisplayName="Void Refinement"),
    ImmortalAscension UMETA(DisplayName="Immortal Ascension"),
    TrueImmortal UMETA(DisplayName="True Immortal"),
    HeavenlyImmortal UMETA(DisplayName="Heavenly Immortal"),
    GoldenImmortal UMETA(DisplayName="Golden Immortal"),
    TrueGod UMETA(DisplayName="True God"),
    PrimordialDeity UMETA(DisplayName="Primordial Deity")
};
