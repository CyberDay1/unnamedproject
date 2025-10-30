#include "Systems/QuestEvents.h"
#include "Components/QuestComponent.h"
#include "GameFramework/Actor.h"

void UQuestEvents::NotifyKill(AActor* Killer, FName TargetID)
{
    if (UQuestComponent* QC = Killer ? Killer->FindComponentByClass<UQuestComponent>() : nullptr)
        QC->OnKill(TargetID);
}
void UQuestEvents::NotifyCollect(AActor* Collector, FName ItemID, int32 Count)
{
    if (UQuestComponent* QC = Collector ? Collector->FindComponentByClass<UQuestComponent>() : nullptr)
        QC->OnCollect(ItemID, Count);
}
void UQuestEvents::NotifyTalk(AActor* Speaker, FName NPCID)
{
    if (UQuestComponent* QC = Speaker ? Speaker->FindComponentByClass<UQuestComponent>() : nullptr)
        QC->OnTalk(NPCID);
}
void UQuestEvents::NotifyReach(AActor* Player, FName LocationTag)
{
    if (UQuestComponent* QC = Player ? Player->FindComponentByClass<UQuestComponent>() : nullptr)
        QC->OnReach(LocationTag);
}
void UQuestEvents::NotifyCraft(AActor* Crafter, FName RecipeID, int32 Count)
{
    if (UQuestComponent* QC = Crafter ? Crafter->FindComponentByClass<UQuestComponent>() : nullptr)
        QC->OnCraft(RecipeID, Count);
}
void UQuestEvents::NotifyDeliver(AActor* Deliverer, FName ItemID, int32 Count, FName ToNPC)
{
    if (UQuestComponent* QC = Deliverer ? Deliverer->FindComponentByClass<UQuestComponent>() : nullptr)
        QC->OnDeliver(ItemID, Count, ToNPC);
}
