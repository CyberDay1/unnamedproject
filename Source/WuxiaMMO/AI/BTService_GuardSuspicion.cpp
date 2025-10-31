#include "AI/BTService_GuardSuspicion.h"
#include "AI/BaseAIController.h"
#include "AI/AIStateComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_GuardSuspicion::UBTService_GuardSuspicion()
{
    Interval = 1.0f;
    RandomDeviation = 0.25f;
}

void UBTService_GuardSuspicion::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ABaseAIController* C = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!C || !C->BB) return;
    APawn* P = C->GetPawn();
    if (!P) return;

    UAIStateComponent* S = P->FindComponentByClass<UAIStateComponent>();
    if (!S) return;

    float Susp = C->BB->GetValueAsFloat(C->KeySuspicion);

    // Simplified: if InTownRegion and target is a different faction (omitted check here), accumulate suspicion
    const bool bInTown = C->BB->GetValueAsBool(C->KeyInTown);
    if (bInTown)
    {
        float Rate = 1.0f;
        for (const FGuardTownRule& R : S->TownRules)
        {
            // If you track regions via volumes, set InTownRegion + switch Rate/Threshold externally
            Rate = FMath::Max(Rate, R.SuspicionPerSecond);
            if (Susp >= R.SuspicionThreshold)
            {
                // Attack: set Engaging; BT should transition to combat subtree
                C->SetBlackboardState(EAIState::Engaging);
                return;
            }
        }
        Susp += Rate * DeltaSeconds;
        C->BB->SetValueAsFloat(C->KeySuspicion, Susp);
    }
    else if (Susp > 0.f)
    {
        Susp = FMath::Max(0.f, Susp - 2.f * DeltaSeconds);
        C->BB->SetValueAsFloat(C->KeySuspicion, Susp);
    }
}
