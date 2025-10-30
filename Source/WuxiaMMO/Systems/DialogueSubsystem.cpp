#include "Systems/DialogueSubsystem.h"

bool UDialogueSubsystem::GetNodeByUUID(const FString& NodeUUID, FDialogueNode& OutNode) const
{
    UDataTable* Table = DialogueTable.LoadSynchronous();
    if (!Table)
    {
        return false;
    }

    for (const auto& Pair : Table->GetRowMap())
    {
        const FDialogueNode* Node = reinterpret_cast<const FDialogueNode*>(Pair.Value);
        if (Node && Node->NodeUUID == NodeUUID)
        {
            OutNode = *Node;
            return true;
        }
    }
    return false;
}

bool UDialogueSubsystem::GetFirstNodeForNPC(const FString& NPCUUID, FDialogueNode& OutNode) const
{
    UDataTable* Table = DialogueTable.LoadSynchronous();
    if (!Table)
    {
        return false;
    }
    for (const auto& Pair : Table->GetRowMap())
    {
        const FDialogueNode* Node = reinterpret_cast<const FDialogueNode*>(Pair.Value);
        if (Node && Node->NPCUUID == NPCUUID)
        {
            OutNode = *Node;
            return true;
        }
    }
    return false;
}
