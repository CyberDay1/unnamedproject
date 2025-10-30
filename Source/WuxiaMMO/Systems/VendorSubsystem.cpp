#include "Systems/VendorSubsystem.h"
#include "Components/CurrencyComponent.h"
#include "Components/ReputationComponent.h"
#include "Components/SpatialRingComponent.h"
#include "Components/CultivationComponent.h"
#include "GameFramework/Actor.h"
#include "Systems/ActionSaveRouterComponent.h"
#include "Items/ItemInstance.h"
#include "HAL/PlatformTime.h"

static double NowSec()
{
    return FPlatformTime::Seconds();
}

int64 UVendorSubsystem::ComputePriceAmount(const FVendorItemDefinition& Def, const UReputationComponent* Rep, FName VendorSectID) const
{
    float Base = static_cast<float>(Def.PriceAmount);
    if (VendorSectID != NAME_None && Rep)
    {
        const float Mult = UCurrencyComponent::PriceMultiplierFromReputation(Rep, VendorSectID, 1.0f);
        Base *= Mult;
    }

    return static_cast<int64>(FMath::Max(0.f, FMath::RoundHalfFromZero(Base)));
}

bool UVendorSubsystem::CanPurchase(const FString& CharacterID, const FVendorItemDefinition& Def, const UCultivationComponent* Cult, const UReputationComponent* Rep, FString& OutError) const
{
    (void)CharacterID;

    if (Cult && static_cast<uint8>(Cult->Progress.Stage) < static_cast<uint8>(Def.MinCultivation))
    {
        OutError = TEXT("Cultivation stage too low for this item.");
        return false;
    }

    if (Rep && Def.RequiredSectID != NAME_None)
    {
        if (Rep->GetSectReputation(Def.RequiredSectID) < Def.MinSectReputation)
        {
            OutError = TEXT("Sect reputation too low.");
            return false;
        }
    }

    OutError.Reset();
    return true;
}

float UVendorSubsystem::GetRemainingCooldownSeconds(const FString& VendorUUID, const FString& CharacterID, const FVendorItemDefinition& Def) const
{
    const FPerCharacterCooldown* VendorMap = VendorCooldowns.Find(VendorUUID);
    if (!VendorMap)
    {
        return 0.f;
    }

    const TMap<FName, double>* StampMap = VendorMap->LastPurchaseTimestamps.Find(CharacterID);
    if (!StampMap)
    {
        return 0.f;
    }

    const double* LastPurchase = StampMap->Find(Def.ItemID);
    if (!LastPurchase)
    {
        return 0.f;
    }

    double Cooldown = 0.0;
    switch (Def.StockType)
    {
        case EVendorStockType::Limited24h:
            Cooldown = 24.0 * 3600.0;
            break;
        case EVendorStockType::Limited7d:
            Cooldown = 7.0 * 24.0 * 3600.0;
            break;
        default:
            return 0.f;
    }

    const double Elapsed = NowSec() - *LastPurchase;
    return static_cast<float>(FMath::Max(0.0, Cooldown - Elapsed));
}

void UVendorSubsystem::StampPurchase(const FString& VendorUUID, const FString& CharacterID, const FVendorItemDefinition& Def)
{
    if (Def.StockType == EVendorStockType::Infinite)
    {
        return;
    }

    FPerCharacterCooldown& Cooldown = VendorCooldowns.FindOrAdd(VendorUUID);
    TMap<FName, double>& StampMap = Cooldown.LastPurchaseTimestamps.FindOrAdd(CharacterID);
    StampMap.Add(Def.ItemID, NowSec());
}

bool UVendorSubsystem::Purchase(const FString& CharacterID, const FVendorItemDefinition& Def, UCurrencyComponent* Currency, USpatialRingComponent* Ring, const UReputationComponent* Rep, FName VendorSectID, FString& OutError)
{
    if (!Currency || !Ring)
    {
        OutError = TEXT("Missing currency or inventory component.");
        return false;
    }

    const float RemainingCooldown = GetRemainingCooldownSeconds(VendorSectID.ToString(), CharacterID, Def);
    if (RemainingCooldown > 0.f)
    {
        OutError = TEXT("Item is on cooldown for this character.");
        return false;
    }

    const int64 FinalPrice = ComputePriceAmount(Def, Rep, VendorSectID);
    if (!Currency->Remove(Def.PriceCurrency, FinalPrice))
    {
        OutError = TEXT("Insufficient funds.");
        return false;
    }

    FItemDefinition ItemDef;
    ItemDef.ItemID = Def.ItemID;
    ItemDef.MaxStack = 999;

    FItemInstance Instance;
    Instance.Definition = ItemDef;
    Instance.Quantity = 1;

    if (!Ring->AddItem(Instance))
    {
        Currency->Add(Def.PriceCurrency, FinalPrice);
        OutError = TEXT("Inventory full.");
        return false;
    }

    StampPurchase(VendorSectID.ToString(), CharacterID, Def);
    OutError.Reset();

    if (AActor* Owner = Ring->GetOwner())
    {
        if (UActionSaveRouterComponent* Router = Owner->FindComponentByClass<UActionSaveRouterComponent>())
        {
            Router->TriggerSave(TEXT("Vendor:Purchase"));
        }
    }

    return true;
}
