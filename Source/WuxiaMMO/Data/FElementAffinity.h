#pragma once
#include "CoreMinimal.h"
#include "EClassAndElementEnums.h"
#include "FElementAffinity.generated.h"

USTRUCT(BlueprintType)
struct FElementAffinity
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Element")
    EElementType ElementType = EElementType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Element")
    float DamageModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Element")
    float DefenseModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Element")
    float QiRegenModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Element")
    FString EffectDescription;
};
