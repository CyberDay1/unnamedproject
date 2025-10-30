#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.h"
#include "CombatInteractionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UCombatInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatInteractionComponent();

    UFUNCTION(BlueprintCallable, Category="Combat")
    void PerformAttack(FName SkillID, AActor* TargetActor);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    USkillComponent* SkillComp;
};
