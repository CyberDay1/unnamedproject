#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DamageTypes.generated.h"

class AActor;

// Core damage categories
UENUM(BlueprintType)
enum class EDamageSchool : uint8
{
    Physical,
    Elemental,
    True
};

// Wuxing elements (+ None for non-elemental physical/true)
UENUM(BlueprintType)
enum class EElementType : uint8
{
    None,
    Fire,
    Water,
    Earth,
    Metal,
    Wood
};

// Full damage packet passed through the resolver
USTRUCT(BlueprintType)
struct FDamageSpec
{
    GENERATED_BODY()

    // Who/what
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TWeakObjectPtr<AActor> Source;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TWeakObjectPtr<AActor> Target;

    // From a skill or effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDamageSchool School = EDamageSchool::Physical;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EElementType Element = EElementType::None;

    // Raw magnitudes BEFORE modifiers (e.g., skill % already applied to source Attack/QiPower)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseAmount = 0.f;   // primary hit

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanCrit = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanDodge = true;

    // Optional extra metadata
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer ContextTags; // e.g., "Damage.Area", "Damage.Backstab"
};

// Result after resolution
USTRUCT(BlueprintType)
struct FDamageResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FinalAmount = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCrit = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDodged = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bBlockedByPvP = false;
};
