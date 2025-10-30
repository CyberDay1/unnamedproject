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
#include "Misc/Guid.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool UCharacterSaveSubsystem::IsValidName(const FString& Name) const
{
    return !Name.IsEmpty() && Name.MatchesWildcard(TEXT("[A-Za-z0-9]*"));
}

bool UCharacterSaveSubsystem::ValidateCharacterName(const FString& FirstName, const FString& LastName, FString& OutError) const
{
    if (!IsValidName(FirstName) || !IsValidName(LastName))
    {
        OutError = TEXT("Names may only contain letters and numbers.");
        return false;
    }
    // TODO: Add unique name lookup from server/database here.
    return true;
}

FCharacterIdentity UCharacterSaveSubsystem::CreateNewCharacter(const FString& FirstName, const FString& LastName, EClassType ClassType, EElementType ElementAffinity)
{
    FCharacterIdentity NewCharacter;
    FString Error;
    if (!ValidateCharacterName(FirstName, LastName, Error))
    {
        UE_LOG(LogTemp, Warning, TEXT("Character creation failed: %s"), *Error);
        return NewCharacter;
    }

    NewCharacter.CharacterID = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    NewCharacter.FirstName = FirstName;
    NewCharacter.LastName = LastName;
    NewCharacter.ClassType = ClassType;
    NewCharacter.ElementAffinity = ElementAffinity;
    NewCharacter.CultivationStage = ECultivationStage::MortalBody;
    NewCharacter.SubStage = 1;
    return NewCharacter;
}

void UCharacterSaveSubsystem::SaveCharacter(const FCharacterIdentity& Identity, const FCharacterStats& Stats)
{
    FString SavePath = FPaths::ProjectSavedDir() / TEXT("Characters");
    FString FilePath = SavePath / (Identity.CharacterID + TEXT(".sav"));

    FString DataString = FString::Printf(
        TEXT("%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"),
        *Identity.CharacterID,
        *Identity.GetFullName(),
        *UEnum::GetValueAsString(Identity.ClassType),
        (int32)Stats.HP,
        (int32)Stats.Qi,
        Stats.Wisdom, Stats.Vitality, Stats.Agility,
        Stats.Willpower, Stats.Luck, Stats.Constitution, Stats.Spirit,
        (int32)Identity.CultivationStage, (int32)Identity.SubStage, (int32)Identity.ElementAffinity
    );

    FFileHelper::SaveStringToFile(DataString, *FilePath);
}

bool UCharacterSaveSubsystem::LoadCharacter(const FString& CharacterID, FCharacterIdentity& OutIdentity, FCharacterStats& OutStats) const
{
    FString SavePath = FPaths::ProjectSavedDir() / TEXT("Characters");
    FString FilePath = SavePath / (CharacterID + TEXT(".sav"));

    FString LoadedData;
    if (!FFileHelper::LoadFileToString(LoadedData, *FilePath))
        return false;

    // TODO: Implement full parsing when DB or backend integration is added.
    UE_LOG(LogTemp, Log, TEXT("Loaded character: %s"), *LoadedData);
    return true;
}
