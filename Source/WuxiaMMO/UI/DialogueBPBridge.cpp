#include "UI/DialogueBPBridge.h"

#include "Components/DialogueComponent.h"

namespace
{
void FillResponseView(const TArray<FDialogueResponse>& InResponses, TArray<FDialogueResponseView>& OutViews)
{
    OutViews.Reset();
    for (const FDialogueResponse& Response : InResponses)
    {
        FDialogueResponseView View;
        View.PlayerLine = Response.PlayerLine;
        View.NextNodeUUID = Response.NextNodeUUID;
        OutViews.Add(View);
    }
}
}

bool UDialogueBPBridge::StartWithNPC(const FString& NPCUUID, FDialogueNode& OutNode, TArray<FDialogueResponseView>& OutResponses)
{
    if (!DialogueComp)
    {
        return false;
    }

    if (!DialogueComp->StartDialogueWithNPC(NPCUUID, OutNode))
    {
        return false;
    }

    CachedResponses = OutNode.Responses;
    FillResponseView(CachedResponses, OutResponses);
    return true;
}

bool UDialogueBPBridge::ChooseByIndex(int32 Index, FDialogueNode& OutNode, TArray<FDialogueResponseView>& OutResponses)
{
    if (!DialogueComp)
    {
        return false;
    }

    if (!CachedResponses.IsValidIndex(Index))
    {
        return false;
    }

    FDialogueNode NextNode;
    if (!DialogueComp->ChooseResponse(CachedResponses[Index], NextNode))
    {
        return false;
    }

    OutNode = NextNode;
    CachedResponses = NextNode.Responses;
    FillResponseView(CachedResponses, OutResponses);
    return true;
}
