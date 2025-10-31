#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EnvEnums.h"
#include "WorldTimeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWorldTimeUpdated, float, HourOfDay, EDayPhase, Phase);

UCLASS()
class UWorldTimeSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // Configuration (in seconds)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Time")
    float RealSecondsPerDayPhase_Day   = 1.5f * 3600.f;   // 1.5 hours

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Time")
    float RealSecondsPerDayPhase_Night = 0.5f * 3600.f;   // 0.5 hours

    // Current time [0..24)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Time")
    float HourOfDay = 8.0f; // default morning

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Time")
    EDayPhase CurrentPhase = EDayPhase::Day;

    UPROPERTY(BlueprintAssignable) FOnWorldTimeUpdated OnTimeUpdated;

    // Tick from world
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category="Time") void AdvanceByRealSeconds(float RealSeconds);
    UFUNCTION(BlueprintPure,   Category="Time") float GetHourOfDay() const { return HourOfDay; }
    UFUNCTION(BlueprintPure,   Category="Time") EDayPhase GetDayPhase() const { return CurrentPhase; }

    // Helper for lighting (normalized sun angle 0..1 across 24h)
    UFUNCTION(BlueprintPure, Category="Time") float GetNormalizedDayFraction() const { return FMath::Fmod(FMath::Max(0.f, HourOfDay), 24.f) / 24.f; }

private:
    FTimerHandle TickHandle;
    void Tick();
    void RecomputePhase();
};
