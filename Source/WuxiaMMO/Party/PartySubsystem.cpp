#include "Party/PartySubsystem.h"

bool UPartySubsystem::GetPartyByCharacter(const FString& CharacterID, FPartyInfo& OutParty) const
{
    if (const FString* PartyID = CharacterToPartyID.Find(CharacterID))
    {
        return GetPartyByID(*PartyID, OutParty);
    }
    return false;
}

bool UPartySubsystem::GetPartyByID(const FString& PartyID, FPartyInfo& OutParty) const
{
    if (const FPartyInfo* Party = PartiesByID.Find(PartyID))
    {
        OutParty = *Party;
        return true;
    }
    return false;
}

void UPartySubsystem::MockSetParty(const FPartyInfo& PartyInfo)
{
    if (PartyInfo.PartyID.IsEmpty())
    {
        return;
    }

    PartiesByID.Add(PartyInfo.PartyID, PartyInfo);
    IndexPartyMembers(PartyInfo);
}

void UPartySubsystem::MockClearParties()
{
    PartiesByID.Empty();
    CharacterToPartyID.Empty();
}

void UPartySubsystem::UpdatePartyState(const FPartyInfo& PartyInfo)
{
    if (PartyInfo.PartyID.IsEmpty())
    {
        return;
    }

    PartiesByID.FindOrAdd(PartyInfo.PartyID) = PartyInfo;
    IndexPartyMembers(PartyInfo);
}

void UPartySubsystem::IndexPartyMembers(const FPartyInfo& PartyInfo)
{
    if (PartyInfo.PartyID.IsEmpty())
    {
        return;
    }

    for (const FPartyMember& Member : PartyInfo.Members)
    {
        if (!Member.CharacterID.IsEmpty())
        {
            CharacterToPartyID.Add(Member.CharacterID, PartyInfo.PartyID);
        }
    }
}
