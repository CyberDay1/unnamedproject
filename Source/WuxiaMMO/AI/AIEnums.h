#pragma once
#include "CoreMinimal.h"
#include "AIEnums.generated.h"

UENUM(BlueprintType)
enum class EAIKind : uint8
{
    Monster,
    Human
};

UENUM(BlueprintType)
enum class EAIState : uint8
{
    Idle,
    Patrol,
    Alert,
    Engaging,
    Enraged,
    Fleeing,
    MovingBack,
    Sleeping,   // Human only (night)
    Distracted, // Human only (events/dialogue)
    Dead,
    Respawning
};
