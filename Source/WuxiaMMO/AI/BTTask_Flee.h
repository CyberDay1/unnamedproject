#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Flee.generated.h"

UCLASS()
class UBTTask_Flee : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UBTTask_Flee();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
