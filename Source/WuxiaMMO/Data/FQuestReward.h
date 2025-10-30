#pragma once
#include "CoreMinimal.h"
#include "FQuestEnums.h"
#include "FItemDefinition.h"
#include "FQuestReward.generated.h"

USTRUCT(BlueprintType)
struct FQuestReward
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reward")
    EQuestRewardType Type = EQuestRewardType::Qi;

    // Qi or Skill/Lifeskill XP amount
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reward")
    float Amount = 0.0f;

    // For LifeSkillXP, specify skill name (e.g., "Alchemy", "Smithing")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reward")
    FName SkillName = NAME_None;

    // For Item rewards (definition key + quantity)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reward")
    FName ItemID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reward")
    int32 Quantity = 1;
};
