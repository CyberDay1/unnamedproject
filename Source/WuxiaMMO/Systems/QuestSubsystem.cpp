#include "Systems/QuestSubsystem.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool UQuestSubsystem::GetDefinitionByUUID(const FString& QuestUUID, FQuestDefinition& OutDef) const
{
    UDataTable* Table = QuestRegistry.LoadSynchronous();
    if (!Table) return false;

    // Scan rows for UUID match
    TArray<FName> RowNames = Table->GetRowNames();
    for (const FName& Row : RowNames)
    {
        FQuestDefinition* Def = Table->FindRow<FQuestDefinition>(Row, TEXT("QuestLookup"));
        if (Def && Def->QuestUUID == QuestUUID)
        {
            OutDef = *Def;
            return true;
        }
    }
    return false;
}

void UQuestSubsystem::SaveQuests(const FString& CharacterID, const TArray<FQuestState>& States)
{
    FString Dir = FPaths::ProjectSavedDir() / TEXT("Quests");
    FString Path = Dir / (CharacterID + TEXT("_Quests.sav"));

    FString Out;
    for (const FQuestState& S : States)
    {
        Out += FString::Printf(TEXT("%s|%d|%d|%d\n"),
            *S.QuestUUID, (int32)S.bAccepted, (int32)S.bCompleted, (int32)S.bTurnedIn);
        // Objective detail serialization omitted in prototype
    }
    FFileHelper::SaveStringToFile(Out, *Path);
}

bool UQuestSubsystem::LoadQuests(const FString& CharacterID, TArray<FQuestState>& OutStates) const
{
    FString Dir = FPaths::ProjectSavedDir() / TEXT("Quests");
    FString Path = Dir / (CharacterID + TEXT("_Quests.sav"));

    FString In;
    if (!FFileHelper::LoadFileToString(In, *Path))
        return false;

    TArray<FString> Lines;
    In.ParseIntoArrayLines(Lines);
    for (const FString& L : Lines)
    {
        TArray<FString> Parts;
        L.ParseIntoArray(Parts, TEXT("|"));
        if (Parts.Num() >= 4)
        {
            FQuestState S;
            S.QuestUUID = Parts[0];
            S.bAccepted = (FCString::Atoi(*Parts[1]) != 0);
            S.bCompleted = (FCString::Atoi(*Parts[2]) != 0);
            S.bTurnedIn = (FCString::Atoi(*Parts[3]) != 0);
            OutStates.Add(S);
        }
    }
    return true;
}
