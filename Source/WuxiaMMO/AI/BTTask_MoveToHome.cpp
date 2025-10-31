#include "AI/BTTask_MoveToHome.h"
#include "AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Navigation/NavigationTypes.h"
#include "AI/Navigation/NavigationSystem.h"

UBTTask_MoveToHome::UBTTask_MoveToHome(){ NodeName = "Move To Home"; }

EBTNodeResult::Type UBTTask_MoveToHome::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABaseAIController* C = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!C || !C->BB) return EBTNodeResult::Failed;
    const FVector Home = C->BB->GetValueAsVector(C->KeyHomeLocation);
    C->MoveToLocation(Home, 50.f);
    C->SetBlackboardState(EAIState::MovingBack);
    return EBTNodeResult::Succeeded;
}
