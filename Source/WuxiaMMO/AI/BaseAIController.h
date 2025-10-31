#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AIEnums.h"
#include "AITypes.h"
#include "BaseAIController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;

UCLASS()
class ABaseAIController : public AAIController
{
    GENERATED_BODY()
public:
    ABaseAIController();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBlackboardComponent* BB;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UAIPerceptionComponent* Perception;
    UPROPERTY() UAISenseConfig_Sight* SightConfig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI") UBehaviorTree* DefaultBehavior;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI") FAIStats AIStats;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI") FAIHomeTerritory Home;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI") EAIKind Kind = EAIKind::Monster;

    // Blackboard keys (canonical names)
    UPROPERTY(EditDefaultsOnly, Category="AI|BB") FName KeyTargetActor   = "TargetActor";
    UPROPERTY(EditDefaultsOnly, Category="AI|BB") FName KeyState        = "AIState";
    UPROPERTY(EditDefaultsOnly, Category="AI|BB") FName KeyHomeLocation = "HomeLocation";
    UPROPERTY(EditDefaultsOnly, Category="AI|BB") FName KeyIsEnraged    = "IsEnraged";
    UPROPERTY(EditDefaultsOnly, Category="AI|BB") FName KeyIsSleeping   = "IsSleeping";
    UPROPERTY(EditDefaultsOnly, Category="AI|BB") FName KeySuspicion    = "Suspicion";
    UPROPERTY(EditDefaultsOnly, Category="AI|BB") FName KeyInTown       = "InTownRegion";

    virtual void OnPossess(APawn* InPawn) override;

    UFUNCTION(BlueprintCallable) void SetBlackboardState(EAIState NewState);
    UFUNCTION(BlueprintCallable) void SetTargetActor(AActor* Target);

    // Dynamic updates from WeatherSubsystem (Blood Moon)
    UFUNCTION(BlueprintCallable) void ApplyBloodMoonAggro(bool bActive);
};
