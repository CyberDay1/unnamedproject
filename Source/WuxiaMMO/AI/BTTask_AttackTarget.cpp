#include "AI/BTTask_AttackTarget.h"

#include "AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Combat/CombatComponent.h"
#include "GameFramework/Pawn.h"

UBTTask_AttackTarget::UBTTask_AttackTarget()
{
    NodeName = TEXT("Attack Target");
}

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
    ABaseAIController* Controller = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!Controller || !Controller->BB)
    {
        return EBTNodeResult::Failed;
    }

    AActor* Target = Cast<AActor>(Controller->BB->GetValueAsObject(Controller->KeyTargetActor));
    if (!Target)
    {
        return EBTNodeResult::Failed;
    }

    if (APawn* Pawn = Controller->GetPawn())
    {
        if (UCombatComponent* CombatComponent = Pawn->FindComponentByClass<UCombatComponent>())
        {
            if (!CombatComponent->PerformSkillByID(TEXT("SwordArc"), Target))
            {
                CombatComponent->PerformBasicAttack(Target);
            }

            Controller->SetBlackboardState(EAIState::Engaging);
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}
