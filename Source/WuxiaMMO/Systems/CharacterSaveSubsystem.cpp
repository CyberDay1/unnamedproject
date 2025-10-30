#include "Systems/CharacterSaveSubsystem.h"

void UCharacterSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    CachedCharacters.Reset();
    CacheConfiguredCharacters();
}

bool UCharacterSaveSubsystem::LoadCharacter(const FString& CharacterID, FCharacterIdentity& OutIdentity, FCharacterStats& OutStats) const
{
    if (const FStoredCharacterData* FoundData = CachedCharacters.Find(CharacterID))
    {
        OutIdentity = FoundData->Identity;
        OutStats = FoundData->Stats;
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("Character ID %s not found in save cache."), *CharacterID);
    return false;
}

void UCharacterSaveSubsystem::RegisterCharacter(const FStoredCharacterData& CharacterData)
{
    if (CharacterData.Identity.CharacterID.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempted to register character with empty ID."));
        return;
    }

    CachedCharacters.Add(CharacterData.Identity.CharacterID, CharacterData);
}

void UCharacterSaveSubsystem::CacheConfiguredCharacters()
{
    for (const FStoredCharacterData& Entry : DefaultCharacterSaves)
    {
        if (Entry.Identity.CharacterID.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("Default character entry is missing an ID."));
            continue;
        }

        CachedCharacters.Add(Entry.Identity.CharacterID, Entry);
    }
}
