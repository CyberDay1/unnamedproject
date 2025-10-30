#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FQuestEnums.h"
#include "FQuestObjective.h"
#include "FQuestReward.h"
#include "FQuestDefinition.generated.h"

// Data-driven quest template
USTRUCT(BlueprintType)
struct FQuestDefinition : public FTableRowBase
{
    GENERATED_BODY()

    // Full UUID for management
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    FString QuestUUID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    FString Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    EQuestAcceptanceType AcceptanceType = EQuestAcceptanceType::NPCDialogue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    EQuestTurnInType TurnInType = EQuestTurnInType::NPC;

    // Optional: issuing and turn-in NPCs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    FName IssuerNPC = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    FName TurnInNPC = NAME_None;

    // Objective chain (all must complete for success)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    TArray<FQuestObjective> Objectives;

    // Rewards (mix allowed)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    TArray<FQuestReward> Rewards;

    // Sect/Core quest tag (affects reward defaults)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    bool bIsSectOrCoreQuest = false;
};
