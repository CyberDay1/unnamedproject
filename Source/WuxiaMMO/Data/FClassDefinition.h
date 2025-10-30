#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EClassAndElementEnums.h"
#include "FClassDefinition.generated.h"

USTRUCT(BlueprintType)
struct FClassDefinition : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Class")
    FString ClassName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Class")
    TArray<FGameplayTag> StartingAbilityTags;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Class")
    EClassType ClassType = EClassType::None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Class")
    EClassType ClassType = EClassType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Class")
    FString ClassName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float BaseHPModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float BaseQiModifier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    TArray<FString> PrimaryStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    TArray<FString> SecondaryStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
    TArray<FString> StartingAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Class")
    FString WeaponType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Class")
    FString Description;
};
