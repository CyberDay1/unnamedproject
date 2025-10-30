#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WuxiaAbilityBase.generated.h"

UCLASS()
class UWuxiaAbilityBase : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UWuxiaAbilityBase();

    UFUNCTION(BlueprintCallable, Category="Ability")
    void ActivateAbilitySimple();
};
