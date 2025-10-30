#pragma once
#include "CoreMinimal.h"
#include "FQuestObjective.h"
#include "FQuestState.generated.h"

// Runtime state tracked per character
USTRUCT(BlueprintType)
struct FQuestState
{
    GENERATED_BODY()

    // Copy of Definition keys + runtime progress
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    FString QuestUUID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    TArray<FQuestObjective> Objectives;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    bool bAccepted = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    bool bCompleted = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    bool bTurnedIn = false;
};
