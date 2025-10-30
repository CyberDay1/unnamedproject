#include "Components/DialogueComponent.h"
#include "Systems/DialogueSubsystem.h"
#include "Components/ReputationComponent.h"
#include "Components/CultivationComponent.h"
#include "Components/QuestComponent.h"
#include "Systems/QuestSubsystem.h"
#include "Data/FQuestDefinition.h"
#include "Data/Reputation/EFaction.h"

UDialogueComponent::UDialogueComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UDialogueComponent::StartDialogueWithNPC(const FString& NPCUUID, FDialogueNode& OutNode)
{
    CurrentNPCUUID = NPCUUID;
    if (UDialogueSubsystem* DS = GetWorld()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
    {
        if (DS->GetFirstNodeForNPC(NPCUUID, OutNode))
        {
            // Check entry conditions
            if (!AreConditionsMet(OutNode.EntryConditions))
            {
                return false;
            }
            CurrentNodeUUID = OutNode.NodeUUID;
            OnNodeShown.Broadcast(OutNode);
            return true;
        }
    }
    return false;
}

bool UDialogueComponent::ChooseResponse(const FDialogueResponse& Response, FDialogueNode& OutNextNode)
{
    if (!AreConditionsMet(Response.Conditions))
    {
        return false;
    }

    if (UDialogueSubsystem* DS = GetWorld()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
    {
        if (DS->GetNodeByUUID(Response.NextNodeUUID, OutNextNode))
        {
            if (!AreConditionsMet(OutNextNode.EntryConditions))
            {
                return false;
            }

            // Handle quest triggers on the node we are ENTERING
            if (OutNextNode.QuestTrigger != EDialogueQuestTrigger::None)
            {
                if (UQuestComponent* QC = GetOwner()->FindComponentByClass<UQuestComponent>())
                {
                    FQuestDefinition Def;
                    if (UQuestSubsystem* QS = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
                    {
                        if (QS->GetDefinitionByUUID(OutNextNode.QuestUUID, Def))
                        {
                            if (OutNextNode.QuestTrigger == EDialogueQuestTrigger::AcceptQuest)
                            {
                                QC->AcceptQuest(Def);
                            }
                            else if (OutNextNode.QuestTrigger == EDialogueQuestTrigger::TurnInQuest)
                            {
                                QC->TurnInQuest(OutNextNode.QuestUUID, Def, /*bInstant*/ true);
                            }
                        }
                    }
                }
            }

            CurrentNodeUUID = OutNextNode.NodeUUID;
            OnNodeShown.Broadcast(OutNextNode);
            return true;
        }
    }
    return false;
}

bool UDialogueComponent::AreConditionsMet(const TArray<FDialogueCondition>& Conditions) const
{
    UCultivationComponent* Cult = GetOwner()->FindComponentByClass<UCultivationComponent>();
    UReputationComponent* Rep = GetOwner()->FindComponentByClass<UReputationComponent>();

    for (const FDialogueCondition& C : Conditions)
    {
        if (C.bCheckCultivation && Cult)
        {
            if (static_cast<uint8>(Cult->Progress.Stage) < static_cast<uint8>(C.MinCultivation))
            {
                return false;
            }
        }
        if (C.FactionTag != "None" && Rep)
        {
            // Map FactionTag to EFaction by name if possible
            EFaction FVal = EFaction::Neutral;
            const FString Tag = C.FactionTag.ToString();
            if (Tag == "Orthodox")
            {
                FVal = EFaction::Orthodox;
            }
            else if (Tag == "DemonicCultivators")
            {
                FVal = EFaction::DemonicCultivators;
            }
            else if (Tag == "HeavenlyDemonSect")
            {
                FVal = EFaction::HeavenlyDemonSect;
            }
            else if (Tag == "BloodCultist")
            {
                FVal = EFaction::BloodCultist;
            }

            if (Rep->GetFactionReputation(FVal) < C.MinFactionRep)
            {
                return false;
            }
        }
        if (C.SectID != NAME_None && Rep)
        {
            if (Rep->GetSectReputation(C.SectID) < C.MinSectRep)
            {
                return false;
            }
        }
    }
    return true;
}
