#include "AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Environment/WeatherSubsystem.h"

ABaseAIController::ABaseAIController()
{
    BB = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BB"));
    Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2200.f;
    SightConfig->LoseSightRadius = 2600.f;
    SightConfig->PeripheralVisionAngleDegrees = 70.f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    Perception->ConfigureSense(*SightConfig);
    Perception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // Init blackboard
    if (DefaultBehavior && UseBlackboard(DefaultBehavior->BlackboardAsset, BB))
    {
        BB->SetValueAsVector(KeyHomeLocation, Home.Center);
        BB->SetValueAsBool(KeyIsEnraged, false);
        BB->SetValueAsBool(KeyIsSleeping, false);
        BB->SetValueAsFloat(KeySuspicion, 0.f);

        RunBehaviorTree(DefaultBehavior);
    }

    // Adjust home center if not set
    if (Home.Center.IsNearlyZero() && InPawn)
    {
        Home.Center = InPawn->GetActorLocation();
        if (BB) BB->SetValueAsVector(KeyHomeLocation, Home.Center);
    }
}

void ABaseAIController::SetBlackboardState(EAIState NewState)
{
    if (BB) BB->SetValueAsEnum(KeyState, static_cast<uint8>(NewState));
}

void ABaseAIController::SetTargetActor(AActor* Target)
{
    if (BB) BB->SetValueAsObject(KeyTargetActor, Target);
}

void ABaseAIController::ApplyBloodMoonAggro(bool bActive)
{
    // Double detection range during Blood Moon
    const float BaseSight = AIStats.SightRadius > 0 ? AIStats.SightRadius : 2200.f;
    if (SightConfig)
    {
        SightConfig->SightRadius = bActive ? BaseSight * 2.f : BaseSight;
        SightConfig->LoseSightRadius = SightConfig->SightRadius * 1.2f;
        Perception->RequestStimuliListenerUpdate();
    }
    if (BB) BB->SetValueAsBool(KeyIsEnraged, bActive);
}
