#pragma once
#include "CoreMinimal.h"
#include "FDialogueEnums.generated.h"

UENUM(BlueprintType)
enum class EDialogueQuestTrigger : uint8
{
    None,
    AcceptQuest,
    TurnInQuest
};

UENUM(BlueprintType)
enum class EDialogueDeliveryType : uint8
{
    Instant,       // show node immediately
    Timed,         // placeholder for timed display
    Cinematic      // placeholder for future camera/vo
};
