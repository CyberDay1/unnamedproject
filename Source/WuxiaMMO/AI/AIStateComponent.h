#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIEnums.h"
#include "AITypes.h"
#include "AIStateComponent.generated.h"

class UWorldTimeSubsystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UAIStateComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI") EAIKind Kind = EAIKind::Monster;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI") FAIStats Stats;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI") FAIHomeTerritory Home;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI") TArray<FGuardTownRule> TownRules;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") EAIState CurrentState = EAIState::Idle;

    UFUNCTION(BlueprintCallable) void SetState(EAIState S) { CurrentState = S; }
    UFUNCTION(BlueprintPure) EAIState GetState() const { return CurrentState; }

    // Suspicion accumulation (guard behavior in towns)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") float Suspicion = 0.f;

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    TWeakObjectPtr<UWorldTimeSubsystem> TimeSys;
};
