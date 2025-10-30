#include "Tests/TestBlueprintLibrary.h"

#include "Components/CultivationComponent.h"
#include "Components/CurrencyComponent.h"
#include "Components/ReputationComponent.h"
#include "ECurrencyType.h"

namespace
{
FTestResult MakeTestResult(const bool bPassed, const FString& Message)
{
    FTestResult Result;
    Result.bPassed = bPassed;
    Result.Message = Message;
    return Result;
}
}

FTestResult UTestBlueprintLibrary::RunCurrencyConversionTests(UCurrencyComponent* Currency)
{
    if (!Currency)
    {
        return MakeTestResult(false, TEXT("CurrencyComponent missing"));
    }

    Currency->Balances.Empty();
    Currency->Add(ECurrencyType::SilverTael, 10);

    const int64 ConvertedUp = Currency->ConvertUp(ECurrencyType::SilverTael, 10);
    if (ConvertedUp != 1)
    {
        return MakeTestResult(false, TEXT("10 Silver did not convert to 1 Gold"));
    }

    if (Currency->Balances.FindRef(ECurrencyType::GoldTael) != 1)
    {
        return MakeTestResult(false, TEXT("Gold balance mismatch"));
    }

    Currency->Balances.Empty();
    Currency->Add(ECurrencyType::ImmortalStone_Poor, 100);

    const int64 ImmortalUp = Currency->ConvertUp(ECurrencyType::ImmortalStone_Poor, 100);
    if (ImmortalUp != 1)
    {
        return MakeTestResult(false, TEXT("100 Poor Immortal should convert to 1 Average Immortal"));
    }

    return MakeTestResult(true, TEXT("Currency conversion tests passed"));
}

FTestResult UTestBlueprintLibrary::RunPriceScalingTests(const UReputationComponent* Rep, FName SectID)
{
    if (!Rep)
    {
        return MakeTestResult(false, TEXT("ReputationComponent missing"));
    }

    constexpr float BasePrice = 1000.f;
    const float Multiplied = UCurrencyComponent::PriceMultiplierFromReputation(Rep, SectID, BasePrice);
    if (!(Multiplied > 0.f && FMath::IsFinite(Multiplied)))
    {
        return MakeTestResult(false, TEXT("Price multiplier invalid"));
    }

    return MakeTestResult(true, FString::Printf(TEXT("Price multiplier ok: %.2f"), Multiplied));
}

FTestResult UTestBlueprintLibrary::RunStoneEffectivenessTests(UCurrencyComponent* Currency, UCultivationComponent* Cult)
{
    if (!Currency || !Cult)
    {
        return MakeTestResult(false, TEXT("Missing Currency or Cultivation"));
    }

    Currency->Balances.Empty();
    Currency->Add(ECurrencyType::SmallSpiritStone_Poor, 1);

    float QiGained = 0.f;
    const bool bConsumed = Currency->ConsumeStoneForQi(ECurrencyType::SmallSpiritStone_Poor, Cult, QiGained);
    if (!bConsumed || QiGained <= 0.f)
    {
        return MakeTestResult(false, TEXT("Stone did not grant Qi at or above tier"));
    }

    return MakeTestResult(true, FString::Printf(TEXT("Stone consumed, Qi gained: %.2f"), QiGained));
}
