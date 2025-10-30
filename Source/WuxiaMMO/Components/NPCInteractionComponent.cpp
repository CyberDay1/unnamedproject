#include "Components/NPCInteractionComponent.h"
#include "Components/DialogueComponent.h"
#include "Data/Dialogue/FDialogueNode.h"

bool UNPCInteractionComponent::BeginDialogue(AActor* PlayerActor)
{
    if (!PlayerActor)
    {
        return false;
    }
    if (UDialogueComponent* DC = PlayerActor->FindComponentByClass<UDialogueComponent>())
    {
        FDialogueNode Node;
        return DC->StartDialogueWithNPC(NPCUUID, Node);
    }
    return false;
}
