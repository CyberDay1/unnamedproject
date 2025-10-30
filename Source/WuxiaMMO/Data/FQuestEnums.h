#pragma once
#include "CoreMinimal.h"
#include "FQuestEnums.generated.h"

UENUM(BlueprintType)
enum class EQuestAcceptanceType : uint8
{
    Auto        UMETA(DisplayName="Auto-Accept"),
    NPCDialogue UMETA(DisplayName="NPC Dialogue")
};

UENUM(BlueprintType)
enum class EQuestTurnInType : uint8
{
    Instant UMETA(DisplayName="Instant"),
    NPC     UMETA(DisplayName="NPC Turn-In")
};

UENUM(BlueprintType)
enum class EQuestObjectiveType : uint8
{
    Kill,
    Collect,
    Talk,
    Reach,
    Craft,
    Deliver
};

UENUM(BlueprintType)
enum class EQuestRewardType : uint8
{
    Qi,
    SkillXP,
    LifeSkillXP,
    Item,
    Currency
};
