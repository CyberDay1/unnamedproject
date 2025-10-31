#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Loot/LootTypes.h"
#include "AuctionHouseSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FAuctionListing
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString ListingID;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> BeneficiaryCharacterIDs;

    UPROPERTY(BlueprintReadOnly)
    FName ItemID;

    UPROPERTY(BlueprintReadOnly)
    int32 Quantity = 1;

    UPROPERTY(BlueprintReadOnly)
    ELootRarity Rarity = ELootRarity::Common;

    UPROPERTY(BlueprintReadOnly)
    int64 BuyoutPrice = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAuctionSold, const FString&, ListingID, int64, FinalPrice);

UCLASS()
class UAuctionHouseSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FAuctionSold OnListingSold;

    UFUNCTION(BlueprintCallable)
    FString CreateListing(const FAuctionListing& Listing);

    UFUNCTION(BlueprintCallable)
    void SimulateImmediateSale(const FString& ListingID, int64 Price);

    bool GetListing(const FString& ListingID, FAuctionListing& OutListing) const;

private:
    TMap<FString, FAuctionListing> ActiveListings;
};
