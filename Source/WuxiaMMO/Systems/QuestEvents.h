#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestEvents.generated.h"

class AActor;

// Simple static dispatcher stub for prototype (replace with proper event bus later)
UCLASS()
class UQuestEvents : public UObject
{
    GENERATED_BODY()
public:
    // Call these from gameplay when things happen (kills, collects, etc.)
    UFUNCTION(BlueprintCallable, Category="QuestEvents")
    static void NotifyKill(AActor* Killer, FName TargetID);

    UFUNCTION(BlueprintCallable, Category="QuestEvents")
    static void NotifyCollect(AActor* Collector, FName ItemID, int32 Count);

    UFUNCTION(BlueprintCallable, Category="QuestEvents")
    static void NotifyTalk(AActor* Speaker, FName NPCID);

    UFUNCTION(BlueprintCallable, Category="QuestEvents")
    static void NotifyReach(AActor* Player, FName LocationTag);

    UFUNCTION(BlueprintCallable, Category="QuestEvents")
    static void NotifyCraft(AActor* Crafter, FName RecipeID, int32 Count);

    UFUNCTION(BlueprintCallable, Category="QuestEvents")
    static void NotifyDeliver(AActor* Deliverer, FName ItemID, int32 Count, FName ToNPC);
};
