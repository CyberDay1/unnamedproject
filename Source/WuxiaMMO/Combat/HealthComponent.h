#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamaged, AActor*, InstigatorActor, float, FinalDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, Killer);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UHealthComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
    float MaxHP = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
    float HP = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
    bool bIsDead = false;

    UPROPERTY(BlueprintAssignable)
    FOnDamaged OnDamaged;

    UPROPERTY(BlueprintAssignable)
    FOnDeath OnDeath;

    UFUNCTION(BlueprintCallable, Category="Health")
    void ApplyDamage(float Amount, AActor* InstigatorActor);

    UFUNCTION(BlueprintCallable, Category="Health")
    void Heal(float Amount);

private:
    void HandleDeath(AActor* Killer);
    void StartFadeAndDestroy();
};
