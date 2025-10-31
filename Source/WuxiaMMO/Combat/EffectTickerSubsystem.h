#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Combat/DamageTypes.h"
#include "EffectTickerSubsystem.generated.h"

USTRUCT()
struct FPendingTickEffect
{
    GENERATED_BODY()

    FDamageSpec Spec;
    float Interval = 1.f;
    float TimeLeft = 5.f;
    float Accum = 0.f;
};

UCLASS()
class UEffectTickerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Combat")
    int32 AddTickEffect(const FDamageSpec& Spec, float IntervalSeconds, float DurationSeconds);

    UFUNCTION(BlueprintCallable, Category="Combat")
    void RemoveTickEffect(int32 Handle);

    virtual void Tick(float DeltaSeconds) override;

    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    virtual TStatId GetStatId() const override;

    virtual bool IsTickable() const override { return true; }

private:
    TMap<int32, FPendingTickEffect> Active;
    int32 NextHandle = 1;
};
