#include "Economy/AuctionHouseSubsystem.h"

#include "Misc/Guid.h"

FString UAuctionHouseSubsystem::CreateListing(const FAuctionListing& Listing)
{
    FAuctionListing Stored = Listing;
    if (Stored.ListingID.IsEmpty())
    {
        Stored.ListingID = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphensInBraces);
    }

    ActiveListings.Add(Stored.ListingID, Stored);
    return Stored.ListingID;
}

void UAuctionHouseSubsystem::SimulateImmediateSale(const FString& ListingID, int64 Price)
{
    FAuctionListing Listing;
    if (!GetListing(ListingID, Listing))
    {
        return;
    }

    ActiveListings.Remove(ListingID);
    OnListingSold.Broadcast(ListingID, Price);
}

bool UAuctionHouseSubsystem::GetListing(const FString& ListingID, FAuctionListing& OutListing) const
{
    if (const FAuctionListing* Found = ActiveListings.Find(ListingID))
    {
        OutListing = *Found;
        return true;
    }
    return false;
}
