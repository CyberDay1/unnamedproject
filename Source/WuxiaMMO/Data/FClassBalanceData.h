#pragma once

#include "CoreMinimal.h"
#include "EClassAndElementEnums.h"
#include "FClassBalanceData.generated.h"

USTRUCT(BlueprintType)
struct FClassBalanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ClassScaling")
    EClassType ClassType = EClassType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ClassScaling")
    float AgilityScale = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ClassScaling")
    float VitalityScale = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ClassScaling")
    float WisdomScale = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ClassScaling")
    float SpiritScale = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ClassScaling")
    float WillpowerScale = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ClassScaling")
    float QiEfficiency = 1.0f;
};
