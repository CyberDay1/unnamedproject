#include "Components/CombatInteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UCombatInteractionComponent::UCombatInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCombatInteractionComponent::PerformAttack(FName SkillID, AActor* TargetActor)
{
    if (!SkillComp || !TargetActor)
    {
        return;
    }

    float QiCost = 0.0f;
    float Damage = 0.0f;
    const bool bUsed = SkillComp->UseSkill(SkillID, 100.0f, QiCost, Damage);
    if (!bUsed)
    {
        return;
    }

    UGameplayStatics::ApplyDamage(TargetActor, Damage, nullptr, GetOwner(), nullptr);
}
