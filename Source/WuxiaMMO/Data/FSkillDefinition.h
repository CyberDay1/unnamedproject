#pragma once

#include "CoreMinimal.h"
#include "EClassAndElementEnums.h"
#include "ECultivationStage.h"
#include "FSkillDefinition.generated.h"

USTRUCT(BlueprintType)
struct FSkillDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    FName SkillID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    EClassType ClassRestriction = EClassType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    EElementType ElementType = EElementType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    float QiCost = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    float Cooldown = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    float BasePower = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    int32 MaxLevelPerRealm = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    ECultivationStage RequiredRealm = ECultivationStage::MortalBody;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    float BaseSkillXP = 100.0f;

    // Combo chaining
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill|Combo")
    FName ComboFrom = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill|Combo")
    FName ComboTo = NAME_None;

    // Input method eligibility
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill|Combo")
    bool bKeyComboEnabled = true;
};
