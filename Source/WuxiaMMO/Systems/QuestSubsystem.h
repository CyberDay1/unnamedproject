#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "FQuestDefinition.h"
#include "FQuestState.h"
#include "QuestSubsystem.generated.h"

UCLASS()
class UQuestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // Registry asset path (DataTable<FQuestDefinition>)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
    TSoftObjectPtr<UDataTable> QuestRegistry;

    // Load quest definition by UUID
    UFUNCTION(BlueprintCallable, Category="Quest")
    bool GetDefinitionByUUID(const FString& QuestUUID, FQuestDefinition& OutDef) const;

    // Persistence stubs
    UFUNCTION(BlueprintCallable, Category="Quest|Persistence")
    void SaveQuests(const FString& CharacterID, const TArray<FQuestState>& States);

    UFUNCTION(BlueprintCallable, Category="Quest|Persistence")
    bool LoadQuests(const FString& CharacterID, TArray<FQuestState>& OutStates) const;
};
