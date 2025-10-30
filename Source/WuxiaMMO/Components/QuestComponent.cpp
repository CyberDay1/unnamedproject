#include "Components/QuestComponent.h"
#include "Systems/QuestSubsystem.h"
#include "Systems/InventorySubsystem.h"
#include "Components/SpatialRingComponent.h"
#include "Components/CultivationComponent.h"
#include "Components/SkillComponent.h"
#include "Items/ItemInstance.h"
#include "Characters/ECultivationStage.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/ActionSaveRouterComponent.h"

static void SaveTriggerFromOwner(UActorComponent* Component, const TCHAR* Reason)
{
    if (!Component)
    {
        return;
    }

    if (AActor* Owner = Component->GetOwner())
    {
        if (UActionSaveRouterComponent* Router = Owner->FindComponentByClass<UActionSaveRouterComponent>())
        {
            Router->TriggerSave(Reason);
        }
    }
}

UQuestComponent::UQuestComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UQuestComponent::AcceptQuest(const FQuestDefinition& Def)
{
    // prevent duplicates
    for (const FQuestState& S : ActiveQuests)
        if (S.QuestUUID == Def.QuestUUID) return false;

    FQuestState NewState;
    NewState.QuestUUID = Def.QuestUUID;
    NewState.Objectives = Def.Objectives;
    NewState.bAccepted = true;
    ActiveQuests.Add(NewState);
    OnQuestUpdated.Broadcast(Def.QuestUUID);
    SaveTriggerFromOwner(this, TEXT("Quest:Accept"));
    return true;
}

bool UQuestComponent::TryAdvanceObjective(FQuestState& State, const TFunction<bool(FQuestObjective&)>& Matcher)
{
    bool bAnyUpdated = false;
    for (FQuestObjective& Obj : State.Objectives)
    {
        if (Obj.bCompleted) continue;
        if (Matcher(Obj))
        {
            if (Obj.RequiredCount <= 0 || ++Obj.CurrentCount >= Obj.RequiredCount)
            {
                Obj.bCompleted = true;
            }
            bAnyUpdated = true;
        }
    }
    if (bAnyUpdated)
    {
        OnQuestUpdated.Broadcast(State.QuestUUID);
        SaveTriggerFromOwner(this, TEXT("Quest:Update"));
    }
    return bAnyUpdated;
}

void UQuestComponent::OnKill(FName TargetID)
{
    for (FQuestState& S : ActiveQuests)
    {
        TryAdvanceObjective(S, [&](FQuestObjective& Obj){
            return Obj.Type == EQuestObjectiveType::Kill && Obj.TargetID == TargetID;
        });
    }
}

void UQuestComponent::OnCollect(FName ItemID, int32 Count)
{
    for (FQuestState& S : ActiveQuests)
    {
        TryAdvanceObjective(S, [&](FQuestObjective& Obj){
            if (Obj.Type != EQuestObjectiveType::Collect || Obj.TargetID != ItemID) return false;
            Obj.CurrentCount = FMath::Min(Obj.RequiredCount, Obj.CurrentCount + Count);
            if (Obj.RequiredCount <= 0 || Obj.CurrentCount >= Obj.RequiredCount)
            {
                Obj.bCompleted = true;
            }
            return true;
        });
    }
}

void UQuestComponent::OnTalk(FName NPCID)
{
    for (FQuestState& S : ActiveQuests)
    {
        TryAdvanceObjective(S, [&](FQuestObjective& Obj){
            return Obj.Type == EQuestObjectiveType::Talk && Obj.TargetID == NPCID;
        });
    }
}

void UQuestComponent::OnReach(FName LocationTag)
{
    for (FQuestState& S : ActiveQuests)
    {
        TryAdvanceObjective(S, [&](FQuestObjective& Obj){
            return Obj.Type == EQuestObjectiveType::Reach && Obj.LocationTag == LocationTag;
        });
    }
}

void UQuestComponent::OnCraft(FName RecipeID, int32 Count)
{
    for (FQuestState& S : ActiveQuests)
    {
        TryAdvanceObjective(S, [&](FQuestObjective& Obj){
            if (Obj.Type != EQuestObjectiveType::Craft || Obj.TargetID != RecipeID) return false;
            Obj.CurrentCount = FMath::Min(Obj.RequiredCount, Obj.CurrentCount + Count);
            if (Obj.RequiredCount <= 0 || Obj.CurrentCount >= Obj.RequiredCount)
            {
                Obj.bCompleted = true;
            }
            return true;
        });
    }
}

void UQuestComponent::OnDeliver(FName ItemID, int32 Count, FName ToNPC)
{
    for (FQuestState& S : ActiveQuests)
    {
        TryAdvanceObjective(S, [&](FQuestObjective& Obj){
            if (Obj.Type != EQuestObjectiveType::Deliver) return false;
            if (Obj.TargetID != ItemID || Obj.DeliverToNPC != ToNPC) return false;
            Obj.CurrentCount = FMath::Min(Obj.RequiredCount, Obj.CurrentCount + Count);
            if (Obj.RequiredCount <= 0 || Obj.CurrentCount >= Obj.RequiredCount)
            {
                Obj.bCompleted = true;
            }
            return true;
        });
    }
}

bool UQuestComponent::IsCompleted(const FString& QuestUUID) const
{
    for (const FQuestState& S : ActiveQuests)
    {
        if (S.QuestUUID == QuestUUID)
        {
            for (const FQuestObjective& O : S.Objectives)
                if (!O.bCompleted) return false;
            return true;
        }
    }
    return false;
}

bool UQuestComponent::TurnInQuest(const FString& QuestUUID, const FQuestDefinition& Def, bool bInstant)
{
    for (FQuestState& S : ActiveQuests)
    {
        if (S.QuestUUID == QuestUUID && !S.bTurnedIn && IsCompleted(QuestUUID))
        {
            S.bCompleted = true;
            if (bInstant || Def.TurnInType == EQuestTurnInType::Instant)
            {
                GrantRewards(GetOwner(), Def);
                S.bTurnedIn = true;
                OnQuestCompleted.Broadcast(QuestUUID);
                SaveTriggerFromOwner(this, TEXT("Quest:Complete"));
                return true;
            }
            // For NPC turn-in, UI/interaction should call this again with bInstant=true
            return true;
        }
    }
    return false;
}

void UQuestComponent::GrantRewards(AActor* OwnerActor, const FQuestDefinition& Def)
{
    // Stubs to integrate with your systems
    // Qi: via CultivationComponent->AddQiFlat(...)
    // Skill/Lifeskill XP: via SkillComponent or future Lifeskill component
    // Items: via InventorySubsystem + SpatialRingComponent

    UCultivationComponent* Cult = OwnerActor ? OwnerActor->FindComponentByClass<UCultivationComponent>() : nullptr;
    USkillComponent* Skills = OwnerActor ? OwnerActor->FindComponentByClass<USkillComponent>() : nullptr;
    USpatialRingComponent* Ring = OwnerActor ? OwnerActor->FindComponentByClass<USpatialRingComponent>() : nullptr;

    for (const FQuestReward& R : Def.Rewards)
    {
        switch (R.Type)
        {
            case EQuestRewardType::Qi:
                if (Cult) Cult->AddQiFlat(R.Amount);
                break;
            case EQuestRewardType::SkillXP:
                if (Skills) Skills->AddSkillXP(FName("Universal"), R.Amount, ECultivationStage::MortalBody); // placeholder
                break;
            case EQuestRewardType::LifeSkillXP:
                // TODO: route to lifeskill subsystem once implemented
                break;
            case EQuestRewardType::Item:
                if (Ring && R.ItemID != NAME_None && R.Quantity > 0)
                {
                    FItemDefinition Defn; Defn.ItemID = R.ItemID; Defn.MaxStack = 999;
                    FItemInstance Inst; Inst.Definition = Defn; Inst.Quantity = R.Quantity;
                    Ring->AddItem(Inst);
                }
                break;
            case EQuestRewardType::Currency:
                // TODO: add currency subsystem later
                break;
        }
    }
}
