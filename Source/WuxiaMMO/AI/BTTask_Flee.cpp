#include "AI/BTTask_Flee.h"
#include "AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Flee::UBTTask_Flee(){ NodeName = "Flee"; }

EBTNodeResult::Type UBTTask_Flee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
    ABaseAIController* C = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!C || !C->BB || !C->GetPawn()) return EBTNodeResult::Failed;
    const FVector From = C->GetPawn()->GetActorLocation();
    const AActor* Threat = Cast<AActor>(C->BB->GetValueAsObject(C->KeyTargetActor));
    const FVector Dir = Threat ? (From - Threat->GetActorLocation()).GetSafeNormal() : FMath::VRand();
    const FVector Dest = From + Dir * 1200.f;
    C->MoveToLocation(Dest, 50.f);
    C->SetBlackboardState(EAIState::Fleeing);
    return EBTNodeResult::Succeeded;
}
