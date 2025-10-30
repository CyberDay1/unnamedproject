#pragma once

#include "CoreMinimal.h"
#include "FCharacterStats.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Stats")
    float Health = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Stats")
    float Qi = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Stats")
    float Stamina = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Stats")
    float AttackPower = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Stats")
    float Defense = 5.f;
};
