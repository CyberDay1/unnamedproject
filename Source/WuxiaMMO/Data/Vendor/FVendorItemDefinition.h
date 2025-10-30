#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/Currency/ECurrencyType.h"
#include "Characters/ECultivationStage.h"
#include "FVendorItemDefinition.generated.h"

UENUM(BlueprintType)
enum class EVendorStockType : uint8
{
    Infinite,
    Limited24h,
    Limited7d
};

USTRUCT(BlueprintType)
struct FVendorItemDefinition : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor")
    FName ItemID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor")
    ECurrencyType PriceCurrency = ECurrencyType::SilverTael;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor")
    int64 PriceAmount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor|Requirements")
    FName RequiredSectID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor|Requirements")
    float MinSectReputation = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor|Requirements")
    ECultivationStage MinCultivation = ECultivationStage::MortalBody;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor|Stock")
    EVendorStockType StockType = EVendorStockType::Infinite;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor|Stock")
    int32 MaxPerCooldown = 1;
};
