#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TestBlueprintLibrary.generated.h"

class UCurrencyComponent;
class UReputationComponent;
class UCultivationComponent;

USTRUCT(BlueprintType)
struct FTestResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    bool bPassed = false;

    UPROPERTY(BlueprintReadOnly)
    FString Message;
};

UCLASS()
class UTestBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Tests")
    static FTestResult RunCurrencyConversionTests(UCurrencyComponent* Currency);

    UFUNCTION(BlueprintCallable, Category = "Tests")
    static FTestResult RunPriceScalingTests(const UReputationComponent* Rep, FName SectID);

    UFUNCTION(BlueprintCallable, Category = "Tests")
    static FTestResult RunStoneEffectivenessTests(UCurrencyComponent* Currency, UCultivationComponent* Cult);
};
