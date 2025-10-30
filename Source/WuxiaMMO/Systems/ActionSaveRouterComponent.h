#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Save/SaveDTOs.h"
#include "ActionSaveRouterComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UActionSaveRouterComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persistence")
    FString AccountID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persistence")
    FString CharacterID;

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void TriggerSave(const FString& ReasonTag);

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void FullSaveSnapshot();

protected:
    virtual void BeginPlay() override;

private:
    FPlayerProfile Snapshot() const;

    UFUNCTION()
    void HandleQuestUpdated(const FString& QuestUUID);

    UFUNCTION()
    void HandleQuestCompleted(const FString& QuestUUID);

    UFUNCTION()
    void HandleSkillLevelUp(FName SkillID, int32 NewLevel);

    UFUNCTION()
    void HandleBreakthrough(ECultivationStage NewStage, uint8 NewSubLevel);
};
