#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
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
};
