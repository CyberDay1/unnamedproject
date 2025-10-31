#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PartySubsystem.generated.h"

class AActor;

USTRUCT(BlueprintType)
struct FPartyMember
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString AccountID;

    UPROPERTY(BlueprintReadOnly)
    FString CharacterID;

    UPROPERTY(BlueprintReadOnly)
    TWeakObjectPtr<AActor> Pawn;
};

USTRUCT(BlueprintType)
struct FPartyInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString PartyID;

    UPROPERTY(BlueprintReadOnly)
    FString MasterAccountID;

    UPROPERTY(BlueprintReadOnly)
    TArray<FPartyMember> Members;

    UPROPERTY(BlueprintReadOnly)
    int32 RoundRobinIndex = 0;
};

UCLASS()
class UPartySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    bool GetPartyByCharacter(const FString& CharacterID, FPartyInfo& OutParty) const;

    UFUNCTION(BlueprintCallable)
    bool GetPartyByID(const FString& PartyID, FPartyInfo& OutParty) const;

    UFUNCTION(BlueprintCallable)
    void MockSetParty(const FPartyInfo& PartyInfo);

    UFUNCTION(BlueprintCallable)
    void MockClearParties();

    void UpdatePartyState(const FPartyInfo& PartyInfo);

private:
    void IndexPartyMembers(const FPartyInfo& PartyInfo);

    TMap<FString, FPartyInfo> PartiesByID;
    TMap<FString, FString> CharacterToPartyID;
};
