#pragma once

#include "UObject/Object.h"
#include "Data/Dialogue/FDialogueNode.h"
#include "Data/Dialogue/FDialogueResponse.h"
#include "DialogueBPBridge.generated.h"

class UDialogueComponent;

USTRUCT(BlueprintType)
struct FDialogueResponseView
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString PlayerLine;

    UPROPERTY(BlueprintReadOnly)
    FString NextNodeUUID;
};

UCLASS(BlueprintType)
class UDialogueBPBridge : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    UDialogueComponent* DialogueComp = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool StartWithNPC(const FString& NPCUUID, FDialogueNode& OutNode, TArray<FDialogueResponseView>& OutResponses);

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool ChooseByIndex(int32 Index, FDialogueNode& OutNode, TArray<FDialogueResponseView>& OutResponses);

    UFUNCTION(BlueprintPure, Category = "Dialogue")
    static FString GetNPCLine(const FDialogueNode& Node)
    {
        return Node.NPCLine;
    }

private:
    TArray<FDialogueResponse> CachedResponses;
};
