#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_BloodMoonEnrage.generated.h"

UCLASS()
class UBTService_BloodMoonEnrage : public UBTService
{
    GENERATED_BODY()
public:
    UBTService_BloodMoonEnrage();
protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
