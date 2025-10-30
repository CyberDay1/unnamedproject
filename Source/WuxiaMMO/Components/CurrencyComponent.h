#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Currency/ECurrencyType.h"
#include "Data/Currency/FCurrencyUnit.h"
#include "CurrencyComponent.generated.h"

class UCultivationComponent;
class UReputationComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UCurrencyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCurrencyComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Currency")
    TMap<ECurrencyType, int64> Balances;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Currency")
    TMap<ECurrencyType, FCurrencyUnit> Units;

    UFUNCTION(BlueprintCallable, Category="Currency")
    void Add(ECurrencyType Type, int64 Amount);

    UFUNCTION(BlueprintCallable, Category="Currency")
    bool Remove(ECurrencyType Type, int64 Amount);

    UFUNCTION(BlueprintCallable, Category="Currency")
    int64 ConvertUp(ECurrencyType FromType, int64 MaxSteps = 1);

    UFUNCTION(BlueprintCallable, Category="Currency")
    int64 ConvertDown(ECurrencyType FromType, int64 UnitsToBreak = 1);

    UFUNCTION(BlueprintCallable, Category="Currency")
    bool ConsumeStoneForQi(ECurrencyType StoneType, UCultivationComponent* Cultivation, float& OutQiGained);

    UFUNCTION(BlueprintCallable, Category="Currency")
    static float PriceMultiplierFromReputation(const UReputationComponent* Rep, FName SectID, float Base = 1.0f);

protected:
    virtual void BeginPlay() override;

private:
    bool GetNextHigher(ECurrencyType Type, ECurrencyType& OutNext) const;
    bool GetNextLower(ECurrencyType Type, ECurrencyType& OutPrev) const;
    int32 GetOrder(ECurrencyType Type) const;
};
