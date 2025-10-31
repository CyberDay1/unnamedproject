#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThreatComponent.generated.h"

class AActor;

USTRUCT()
struct FThreatEntry
{
    GENERATED_BODY()

    UPROPERTY()
    TWeakObjectPtr<AActor> Actor;

    UPROPERTY()
    float Value = 0.f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UThreatComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UThreatComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Threat") float TankThreatMultiplier = 5.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Threat") float HealerThreatMultiplier = 2.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Threat") float BaseThreatPerDamage = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Threat") float ThreatDecayPerSecond = 5.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Threat") float LeashResetSeconds = 10.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Threat") float PatrolRadius = 2500.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Threat") FVector HomeLocation = FVector::ZeroVector;

    UFUNCTION(BlueprintCallable, Category="Threat") void AddDamageThreat(AActor* Instigator, float DamageAmount, bool bInstigatorIsTank = false);
    UFUNCTION(BlueprintCallable, Category="Threat") void AddHealThreat(AActor* Healer, float HealAmount);
    UFUNCTION(BlueprintCallable, Category="Threat") void DecayThreat(float DeltaSeconds);
    UFUNCTION(BlueprintCallable, Category="Threat") void ResetThreat();
    UFUNCTION(BlueprintPure, Category="Threat") AActor* GetTopThreatActor() const;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void AccumulateThreat(AActor* SourceActor, float Amount);

    UPROPERTY()
    TArray<FThreatEntry> Entries;

    float TimeOutsidePatrol = 0.f;
};
