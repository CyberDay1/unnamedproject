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
    // Base stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float HP = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float Qi = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float MovementSpeed = 600.0f; // Placeholder value, cm/s

    // Core attributes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    int32 Wisdom = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    int32 Vitality = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    int32 Agility = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    int32 Willpower = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    int32 Luck = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    int32 Constitution = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    int32 Spirit = 5;

    void ApplyGrowthFormulas()
    {
        HP = 100.0f + (Vitality * 10.0f) + (Constitution * 3.0f);
        Qi = 10.0f + (Wisdom * 1.5f) + (Spirit * 1.0f);
    }
};
