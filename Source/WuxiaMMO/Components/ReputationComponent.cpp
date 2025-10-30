#include "Components/ReputationComponent.h"

UReputationComponent::UReputationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UReputationComponent::AddFactionReputation(EFaction Faction, float Amount)
{
    float& Ref = FactionReputation.FindOrAdd(Faction);
    Ref = FMath::Clamp(Ref + Amount, -10000.0f, 10000.0f);
}

float UReputationComponent::GetFactionReputation(EFaction Faction) const
{
    if (const float* Val = FactionReputation.Find(Faction))
    {
        return *Val;
    }
    return 0.0f;
}

FSectReputation* UReputationComponent::FindSect(FName SectID)
{
    for (FSectReputation& S : SectReps)
    {
        if (S.SectID == SectID)
        {
            return &S;
        }
    }
    return nullptr;
}

void UReputationComponent::AddSectReputation(FName SectID, float Amount)
{
    if (FSectReputation* S = FindSect(SectID))
    {
        S->Reputation = FMath::Clamp(S->Reputation + Amount, -10000.0f, 10000.0f);
    }
    else
    {
        FSectReputation NewRep;
        NewRep.SectID = SectID;
        NewRep.Reputation = FMath::Clamp(Amount, -10000.0f, 10000.0f);
        SectReps.Add(NewRep);
    }
}

float UReputationComponent::GetSectReputation(FName SectID) const
{
    for (const FSectReputation& S : SectReps)
    {
        if (S.SectID == SectID)
        {
            return S.Reputation;
        }
    }
    return 0.0f;
}
