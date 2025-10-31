#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackTarget.generated.h"

// Calls into existing combat framework (trace/VFX). Replace stub callback as you wire combat.
UCLASS()
class UBTTask_AttackTarget : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UBTTask_AttackTarget();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
