#include "Abilities/WuxiaAbilityBase.h"

UWuxiaAbilityBase::UWuxiaAbilityBase()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UWuxiaAbilityBase::ActivateAbilitySimple()
{
    K2_CommitAbility();
}
