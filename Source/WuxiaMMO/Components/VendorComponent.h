#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Vendor/FVendorItemDefinition.h"
#include "VendorComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UVendorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVendorComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor")
    FString VendorUUID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor")
    TSoftObjectPtr<class UDataTable> VendorInventoryTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor")
    bool bReputationAffectsPrice = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor")
    FName SectID = NAME_None;
};
