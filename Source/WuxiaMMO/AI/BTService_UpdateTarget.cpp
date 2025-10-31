#include "AI/BTService_UpdateTarget.h"
#include "AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Components/ReputationComponent.h"
#include "Data/Reputation/EFaction.h"

UBTService_UpdateTarget::UBTService_UpdateTarget()
{
    bNotifyBecomeRelevant = true;
    Interval = 0.4f;
    RandomDeviation = 0.1f;
}

static bool AreHostile(const AActor* Self, const AActor* Other)
{
    // Default: different factions are hostile, same faction friendly, Neutral depends
    const UReputationComponent* A = Self ? Self->FindComponentByClass<UReputationComponent>() : nullptr;
    const UReputationComponent* B = Other ? Other->FindComponentByClass<UReputationComponent>() : nullptr;
    if (!A || !B) return false;

    // Simplified: check max faction standing sign
    // Self hostile to Other if their top factions differ and are not Neutral
    // Designers can extend with sect-specific checks later.
    return true; // Will be refined by actual game rules via perception stimulus tags
}

void UBTService_UpdateTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float)
{
    ABaseAIController* C = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!C || !C->Perception || !C->BB) return;

    TArray<AActor*> Perceived;
    C->Perception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), Perceived);

    AActor* Best = nullptr;
    float BestDistSq = TNumericLimits<float>::Max();

    const FVector Origin = C->GetPawn() ? C->GetPawn()->GetActorLocation() : FVector::ZeroVector;

    for (AActor* A : Perceived)
    {
        if (!A) continue;
        if (!AreHostile(C->GetPawn(), A)) continue;

        const float D2 = FVector::DistSquared(Origin, A->GetActorLocation());
        if (D2 < BestDistSq)
        {
            BestDistSq = D2;
            Best = A;
        }
    }
    if (Best)
    {
        C->SetTargetActor(Best);
        C->SetBlackboardState(EAIState::Engaging);
    }
}
