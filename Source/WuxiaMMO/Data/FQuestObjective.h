#pragma once
#include "CoreMinimal.h"
#include "FQuestEnums.h"
#include "FQuestObjective.generated.h"

USTRUCT(BlueprintType)
struct FQuestObjective
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    EQuestObjectiveType Type = EQuestObjectiveType::Kill;

    // Generic fields used per objective type:
    // Kill: TargetID=NPC/Monster ID, RequiredCount
    // Collect: TargetID=ItemID, RequiredCount
    // Talk: TargetID=NPC ID
    // Reach: LocationTag or VolumeName
    // Craft: TargetID=RecipeID, RequiredCount
    // Deliver: TargetID=ItemID, RequiredCount, DeliverToNPC
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    FName TargetID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    int32 RequiredCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    FName LocationTag = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    FName DeliverToNPC = NAME_None;

    // Progress tracking
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    int32 CurrentCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    bool bCompleted = false;
};
