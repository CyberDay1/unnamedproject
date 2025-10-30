#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "FDialogueNode.h"
#include "DialogueSubsystem.generated.h"

UCLASS()
class UDialogueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // Assign in project settings or GameInstance: DataTable of FDialogueNode
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    TSoftObjectPtr<UDataTable> DialogueTable;

    UFUNCTION(BlueprintCallable, Category="Dialogue")
    bool GetNodeByUUID(const FString& NodeUUID, FDialogueNode& OutNode) const;

    UFUNCTION(BlueprintCallable, Category="Dialogue")
    bool GetFirstNodeForNPC(const FString& NPCUUID, FDialogueNode& OutNode) const; // convenience
};
