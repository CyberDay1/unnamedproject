#pragma once
#include "CoreMinimal.h"
#include "FSectReputation.generated.h"

USTRUCT(BlueprintType)
struct FSectReputation
{
    GENERATED_BODY()

    // Player/NPC sect identifier (UUID or name)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reputation")
    FName SectID = NAME_None;

    // Range -10000..+10000
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reputation")
    float Reputation = 0.0f;
};
