#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Vendor/FVendorItemDefinition.h"
#include "VendorSubsystem.generated.h"

class UCurrencyComponent;
class UReputationComponent;
class USpatialRingComponent;
class UCultivationComponent;

USTRUCT()
struct FPerCharacterCooldown
{
    GENERATED_BODY()

    TMap<FString, TMap<FName, double>> LastPurchaseTimestamps;
};

UCLASS()
class UVendorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Vendor")
    int64 ComputePriceAmount(const FVendorItemDefinition& Def, const UReputationComponent* Rep, FName VendorSectID) const;

    UFUNCTION(BlueprintCallable, Category="Vendor")
    bool CanPurchase(const FString& CharacterID, const FVendorItemDefinition& Def, const UCultivationComponent* Cult, const UReputationComponent* Rep, FString& OutError) const;

    UFUNCTION(BlueprintCallable, Category="Vendor")
    bool Purchase(const FString& CharacterID, const FVendorItemDefinition& Def, UCurrencyComponent* Currency, USpatialRingComponent* Ring, const UReputationComponent* Rep, FName VendorSectID, FString& OutError);

private:
    UPROPERTY()
    TMap<FString, FPerCharacterCooldown> VendorCooldowns;

public:
    UFUNCTION(BlueprintCallable, Category="Vendor")
    float GetRemainingCooldownSeconds(const FString& VendorUUID, const FString& CharacterID, const FVendorItemDefinition& Def) const;

    UFUNCTION(BlueprintCallable, Category="Vendor")
    void StampPurchase(const FString& VendorUUID, const FString& CharacterID, const FVendorItemDefinition& Def);
};
