#pragma once
#include "CoreMinimal.h"
#include "ECultivationStage.h"
#include "FDialogueCondition.generated.h"

USTRUCT(BlueprintType)
struct FDialogueCondition
{
    GENERATED_BODY()

    // Cultivation condition
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition")
    bool bCheckCultivation = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition", meta=(EditCondition="bCheckCultivation"))
    ECultivationStage MinCultivation = ECultivationStage::MortalBody;

    // Faction check: if FactionTag != "None", require MinFactionRep
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition")
    FName FactionTag = "None"; // must match EFaction name or external faction key

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition", meta=(ClampMin="-10000.0", ClampMax="10000.0"))
    float MinFactionRep = 0.0f;

    // Sect check (player or NPC sects) by SectID (UUID/name)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition")
    FName SectID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition", meta=(ClampMin="-10000.0", ClampMax="10000.0"))
    float MinSectRep = 0.0f;
};
