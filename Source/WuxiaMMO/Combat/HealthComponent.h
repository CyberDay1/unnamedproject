#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "HealthComponent.generated.h"

UDELEGATE(BlueprintAuthorityOnly) DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, NewHP, float, MaxHP, AActor*, InstigatorActor);
UDELEGATE(BlueprintAuthorityOnly) DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, Killer);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UHealthComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health") float BaseHP = 10.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health") float CurrentHP = 10.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health") float MaxHP = 10.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health") bool bIsDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling") float ClassHPBonus = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling") float VitalityContribution = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling") float ConstitutionMinorBonus = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling") int32 MinorRealmLevel = 0;

    UPROPERTY(BlueprintAssignable, Category="Health") FOnHealthChanged OnHealthChanged;
    UPROPERTY(BlueprintAssignable, Category="Health") FOnDeath OnDeath;

    UFUNCTION(BlueprintCallable, Category="Health") void InitializeHP();
    UFUNCTION(BlueprintCallable, Category="Health") void ApplyDamage(float Amount, AActor* InstigatorActor);
    UFUNCTION(BlueprintCallable, Category="Health") void Heal(float Amount, AActor* InstigatorActor);
    UFUNCTION(BlueprintCallable, Category="Health") void ForceDeath(AActor* Killer);
    UFUNCTION(BlueprintCallable, Category="Health") void RespawnAtLocation(const FVector& WorldLocation);

protected:
    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        InitializeHP();
    }

private:
    void RecalcMaxHP();
};
