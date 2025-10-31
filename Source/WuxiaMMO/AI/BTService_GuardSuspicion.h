#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GuardSuspicion.generated.h"

// Increases suspicion on players of other factions inside town regions. Attacks when threshold reached.
UCLASS()
class UBTService_GuardSuspicion : public UBTService
{
    GENERATED_BODY()
public:
    UBTService_GuardSuspicion();
protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
