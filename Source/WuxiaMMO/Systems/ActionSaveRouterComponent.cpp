#include "Systems/ActionSaveRouterComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameInstance.h"
#include "Systems/PersistenceSubsystem.h"
#include "Components/CultivationComponent.h"
#include "Components/QuestComponent.h"
#include "Components/SkillComponent.h"

void UActionSaveRouterComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* OwnerActor = GetOwner())
    {
        if (UQuestComponent* QuestComponent = OwnerActor->FindComponentByClass<UQuestComponent>())
        {
            QuestComponent->OnQuestUpdated.AddDynamic(this, &UActionSaveRouterComponent::HandleQuestUpdated);
            QuestComponent->OnQuestCompleted.AddDynamic(this, &UActionSaveRouterComponent::HandleQuestCompleted);
        }

        if (USkillComponent* SkillComponent = OwnerActor->FindComponentByClass<USkillComponent>())
        {
            SkillComponent->OnSkillLevelUp.AddDynamic(this, &UActionSaveRouterComponent::HandleSkillLevelUp);
        }

        if (UCultivationComponent* CultivationComponent = OwnerActor->FindComponentByClass<UCultivationComponent>())
        {
            CultivationComponent->OnBreakthrough.AddDynamic(this, &UActionSaveRouterComponent::HandleBreakthrough);
        }
    }
}

void UActionSaveRouterComponent::TriggerSave(const FString& ReasonTag)
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            if (UPersistenceSubsystem* Persistence = GameInstance->GetSubsystem<UPersistenceSubsystem>())
            {
                FPlayerProfile ProfileSnapshot = Snapshot();
                Persistence->QueueSave(ProfileSnapshot, ReasonTag);
            }
        }
    }
}

void UActionSaveRouterComponent::FullSaveSnapshot()
{
    TriggerSave(TEXT("Manual/Full"));
}

FPlayerProfile UActionSaveRouterComponent::Snapshot() const
{
    if (const UWorld* World = GetWorld())
    {
        if (const UGameInstance* GameInstance = World->GetGameInstance())
        {
            if (const UPersistenceSubsystem* Persistence = GameInstance->GetSubsystem<UPersistenceSubsystem>())
            {
                return Persistence->GatherProfileSnapshot(GetOwner(), AccountID, CharacterID);
            }
        }
    }

    return FPlayerProfile{};
}

void UActionSaveRouterComponent::HandleQuestUpdated(const FString& QuestUUID)
{
    TriggerSave(FString::Printf(TEXT("QuestUpdated:%s"), *QuestUUID));
}

void UActionSaveRouterComponent::HandleQuestCompleted(const FString& QuestUUID)
{
    TriggerSave(FString::Printf(TEXT("QuestCompleted:%s"), *QuestUUID));
}

void UActionSaveRouterComponent::HandleSkillLevelUp(FName SkillID, int32 NewLevel)
{
    TriggerSave(FString::Printf(TEXT("SkillLevelUp:%s:%d"), *SkillID.ToString(), NewLevel));
}

void UActionSaveRouterComponent::HandleBreakthrough(ECultivationStage NewStage, uint8 NewSubLevel)
{
    TriggerSave(FString::Printf(TEXT("Breakthrough:%s:%d"), *UEnum::GetValueAsString(NewStage), NewSubLevel));
}
