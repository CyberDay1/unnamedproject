#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToHome.generated.h"

UCLASS()
class UBTTask_MoveToHome : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UBTTask_MoveToHome();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
