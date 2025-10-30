#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Dialogue/FDialogueNode.h"
#include "Data/Dialogue/FDialogueResponse.h"
#include "DialogueComponent.generated.h"

class UDialogueSubsystem;
class UReputationComponent;
class UCultivationComponent;
class UQuestComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueNodeShown, const FDialogueNode&, Node);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UDialogueComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UDialogueComponent();

    UPROPERTY(BlueprintAssignable)
    FOnDialogueNodeShown OnNodeShown;

    UFUNCTION(BlueprintCallable, Category="Dialogue")
    bool StartDialogueWithNPC(const FString& NPCUUID, FDialogueNode& OutNode);

    UFUNCTION(BlueprintCallable, Category="Dialogue")
    bool ChooseResponse(const FDialogueResponse& Response, FDialogueNode& OutNextNode);

    // Runtime context
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dialogue")
    FString CurrentNPCUUID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dialogue")
    FString CurrentNodeUUID;

private:
    bool AreConditionsMet(const TArray<FDialogueCondition>& Conditions) const;
};
