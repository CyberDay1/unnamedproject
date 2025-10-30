#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FQuestState.h"
#include "FQuestDefinition.h"
#include "FQuestEnums.h"
#include "Templates/Function.h"
#include "QuestComponent.generated.h"

class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestUpdated, const FString&, QuestUUID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, const FString&, QuestUUID);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UQuestComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UQuestComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    TArray<FQuestState> ActiveQuests;

    UPROPERTY(BlueprintAssignable)
    FOnQuestUpdated OnQuestUpdated;

    UPROPERTY(BlueprintAssignable)
    FOnQuestCompleted OnQuestCompleted;

    // Accept by definition (auto) or via NPC flow
    UFUNCTION(BlueprintCallable, Category="Quest")
    bool AcceptQuest(const FQuestDefinition& Def);

    // Mark objective progress via event hooks
    void OnKill(FName TargetID);
    void OnCollect(FName ItemID, int32 Count);
    void OnTalk(FName NPCID);
    void OnReach(FName LocationTag);
    void OnCraft(FName RecipeID, int32 Count);
    void OnDeliver(FName ItemID, int32 Count, FName ToNPC);

    // Check completion & turn-in
    UFUNCTION(BlueprintCallable, Category="Quest")
    bool IsCompleted(const FString& QuestUUID) const;

    UFUNCTION(BlueprintCallable, Category="Quest")
    bool TurnInQuest(const FString& QuestUUID, const FQuestDefinition& Def, bool bInstant=false);

private:
    bool TryAdvanceObjective(FQuestState& State, const TFunction<bool(FQuestObjective&)>& Matcher);
    void GrantRewards(AActor* OwnerActor, const FQuestDefinition& Def);
};
