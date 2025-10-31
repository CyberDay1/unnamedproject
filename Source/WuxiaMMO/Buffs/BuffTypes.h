#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "BuffTypes.generated.h"

UENUM(BlueprintType)
enum class EBuffApplication : uint8
{
    Add,
    MaxOverride
};

USTRUCT(BlueprintType)
struct FBuffDefinition : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BuffID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag BuffTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DurationSeconds = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxStacks = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBuffApplication Application = EBuffApplication::Add;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float QiCostMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DefenseMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MoveSpeedMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bBlocksCasting = false;
};

USTRUCT(BlueprintType)
struct FActiveBuff
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BuffID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag BuffTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeRemaining = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Stacks = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float QiCostMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DefenseMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MoveSpeedMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bBlocksCasting = false;
};
