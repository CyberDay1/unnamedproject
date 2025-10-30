#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FDialogueEnums.h"
#include "FDialogueResponse.h"
#include "FDialogueNode.generated.h"

// DataTable row: one dialogue node
USTRUCT(BlueprintType)
struct FDialogueNode : public FTableRowBase
{
    GENERATED_BODY()

    // Unique node UUID (string form)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    FString NodeUUID;

    // Owning NPC UUID (string form)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    FString NPCUUID;

    // The line the NPC speaks
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    FString NPCLine;

    // Optional VO/Anim/Cam placeholders
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue|Meta")
    FString VoiceCue; // placeholder
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue|Meta")
    FString AnimTag;  // placeholder
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue|Meta")
    EDialogueDeliveryType Delivery = EDialogueDeliveryType::Instant;

    // Optional quest trigger
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue|Quest")
    EDialogueQuestTrigger QuestTrigger = EDialogueQuestTrigger::None;

    // If quest trigger is set, associate QuestUUID (string)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue|Quest")
    FString QuestUUID;

    // Responses player can choose from
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    TArray<FDialogueResponse> Responses;

    // Conditions to show THIS node at all
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    TArray<FDialogueCondition> EntryConditions;
};
