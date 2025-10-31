#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class EElementType : uint8
{
    None,
    Metal,
    Wood,
    Water,
    Fire,
    Earth
};

USTRUCT(BlueprintType)
struct FSkillRow
{
    GENERATED_BODY()

    // RowName = SkillID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseDamage = 10.f;         // flat base

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StatMultiplier = 1.0f;     // percent scale (e.g., 1.25 = +125% of scaling stat)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EElementType Element = EElementType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CooldownSeconds = 4.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Range = 220.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius = 60.f;             // sphere trace radius

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName NiagaraSystemName;         // optional VFX
};

USTRUCT(BlueprintType)
struct FDamageEventData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RawDamage = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EElementType Element = EElementType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsSkill = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillID;
};
