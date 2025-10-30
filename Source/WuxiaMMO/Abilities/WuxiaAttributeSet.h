#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "WuxiaAttributeSet.generated.h"

UCLASS()
class UWuxiaAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UWuxiaAttributeSet();

    UPROPERTY(BlueprintReadOnly, Category="Attributes")
    FGameplayAttributeData Health;

    UPROPERTY(BlueprintReadOnly, Category="Attributes")
    FGameplayAttributeData Qi;

    UPROPERTY(BlueprintReadOnly, Category="Attributes")
    FGameplayAttributeData Stamina;
};
