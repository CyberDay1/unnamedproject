#include "Systems/CharacterLoadManager.h"

#include "Characters/WuxiaCharacterBase.h"
#include "Systems/CharacterSaveSubsystem.h"
#include "Kismet/GameplayStatics.h"

ACharacter* UCharacterLoadManager::SpawnCharacterFromSave(const FString& CharacterID, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance)
    {
        return nullptr;
    }

    UCharacterSaveSubsystem* SaveSubsystem = GameInstance->GetSubsystem<UCharacterSaveSubsystem>();
    if (!SaveSubsystem)
    {
        return nullptr;
    }

    FCharacterIdentity Identity;
    FCharacterStats Stats;
    if (!SaveSubsystem->LoadCharacter(CharacterID, Identity, Stats))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load character %s"), *CharacterID);
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AWuxiaCharacterBase* NewCharacter = World->SpawnActor<AWuxiaCharacterBase>(AWuxiaCharacterBase::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
    if (NewCharacter)
    {
        NewCharacter->InitializeFromData(Identity, Stats);
    }

    return NewCharacter;
}
