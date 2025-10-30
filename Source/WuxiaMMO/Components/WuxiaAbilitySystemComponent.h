#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WuxiaAbilitySystemComponent.generated.h"

UCLASS()
class UWuxiaAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UWuxiaAbilitySystemComponent();

    UFUNCTION(BlueprintCallable, Category="Abilities")
    void InitializeDefaultAbilities();
};
