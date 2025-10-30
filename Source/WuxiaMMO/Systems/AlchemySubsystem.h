#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FAlchemyModifier.h"
#include "FCauldronData.h"
#include "AlchemySubsystem.generated.h"

UENUM(BlueprintType)
enum class EPillTier : uint8
{
    Tier1 = 1,
    Tier2,
    Tier3,
    Tier4,
    Tier5
};

UENUM(BlueprintType)
enum class EItemQuality : uint8
{
    Poor = 0,
    Mediocre,
    Normal,
    Greater,
    Rare,
    Legendary,
    Immortal
};

USTRUCT(BlueprintType)
struct FPillRecipe
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
    FName PillName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
    EPillTier Tier = EPillTier::Tier1;

    // Baseline craft success before skill and modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
    float BaseSuccessChance = 0.7f; // 70%

    // If produced, this quality baseline will be multiplied by quality factors and modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
    float BaseQualityMultiplier = 1.0f;

    // If consumed, acts as Qi gain definition: choose one
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
    bool bGrantsPercentOfRequirement = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
    float QiGrantValue = 10.0f; // flat or percent based on flag
};

UCLASS()
class UAlchemySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Calculates crafting outcome.
    // Inputs:
    //  - AlchemyLevel: player skill
    //  - Recipe: pill definition
    //  - ActiveModifiers: temporary item-based boosts
    //  - Cauldron: durability and passive bonuses
    // Outputs:
    //  - bSuccess: whether craft succeeded
    //  - OutQuality: final quality tier
    //  - DurabilityDelta: negative on use
    UFUNCTION(BlueprintCallable, Category="Alchemy")
    void CraftPill(int32 AlchemyLevel, const FPillRecipe& Recipe, const TArray<FAlchemyModifier>& ActiveModifiers,
                   FCauldronData& Cauldron, bool& bSuccess, EItemQuality& OutQuality, float& DurabilityDelta);

    // Returns a multiplier for a quality tier
    UFUNCTION(BlueprintPure, Category="Alchemy")
    static float QualityTierMultiplier(EItemQuality Q);

private:
    float ApplySuccessChance(int32 AlchemyLevel, const FPillRecipe& Recipe, const TArray<FAlchemyModifier>& Modifiers, const FCauldronData& Cauldron) const;
    float RollQualityMultiplier(float Base, const TArray<FAlchemyModifier>& Modifiers, const FCauldronData& Cauldron) const;
};
