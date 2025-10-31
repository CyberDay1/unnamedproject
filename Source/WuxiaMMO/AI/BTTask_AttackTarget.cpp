#include "AI/BTTask_AttackTarget.h"
#include "AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AttackTarget::UBTTask_AttackTarget(){ NodeName = "Attack Target (Stub)"; }

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
    ABaseAIController* C = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!C || !C->BB) return EBTNodeResult::Failed;
    AActor* Target = Cast<AActor>(C->BB->GetValueAsObject(C->KeyTargetActor));
    if (!Target) return EBTNodeResult::Failed;

    // TODO: Invoke your combat system here (e.g., C->GetPawn()->FindComponentByClass<UCombatComponent>()->PerformAttack(Target))
    C->SetBlackboardState(EAIState::Engaging);
    return EBTNodeResult::Succeeded;
}
