#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FCultivationProgress.h"
#include "ECultivationStage.h"
#include "CultivationComponent.generated.h"

// Delegate fired on successful breakthrough (stage advanced, sub-level reset)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBreakthrough, ECultivationStage, NewStage, uint8, NewSubLevel);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UCultivationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCultivationComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation|State")
    FCultivationProgress Progress;

    // Tuning constants for Qi requirement curve
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation|Config")
    float BaseQi = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation|Config")
    float StageMultiplier = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation|Config")
    float SubLevelMultiplier = 1.25f;

    // Passive Qi per second (prototype)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation|Config")
    float PassiveQiPerSec = 0.1f;

    // Called to re-compute RequiredQi for current Stage/SubLevel
    UFUNCTION(BlueprintCallable, Category="Cultivation")
    void RecomputeRequirement();

    // Qi gain hooks (blocked if at cap on SubLevel 9)
    UFUNCTION(BlueprintCallable, Category="Cultivation")
    void AddQiFlat(float Amount);

    // Adds a % of the REQUIRED Qi for current sub-level
    UFUNCTION(BlueprintCallable, Category="Cultivation")
    void AddQiPercentOfRequirement(float Percent); // e.g., 0.10 = 10%

    // Pill consumption: applies quality multiplier and pill-name efficiency decay (1% per same-name use)
    UFUNCTION(BlueprintCallable, Category="Cultivation")
    void ConsumePill(const FName& PillName, float BaseQiOrPercent, bool bIsPercent, float QualityMultiplier);

    // Meditation minigame result applies flat Qi
    UFUNCTION(BlueprintCallable, Category="Cultivation")
    void ApplyMeditationQi(float Amount);

    // Called per second externally or via timer to apply passive Qi
    UFUNCTION(BlueprintCallable, Category="Cultivation")
    void TickPassiveQi(float DeltaSeconds);

    // Attempts breakthrough; requires full Qi and an external item gate when SubLevel==9
    UFUNCTION(BlueprintCallable, Category="Cultivation")
    bool TryBreakthrough(bool bHasStageCatalystItem);

    // Event
    UPROPERTY(BlueprintAssignable)
    FOnBreakthrough OnBreakthrough;

    // Utility
    UFUNCTION(BlueprintPure, Category="Cultivation")
    bool IsQiGainBlocked() const { return Progress.bQiGainBlockedAtCap; }

private:
    float StageIndex(ECultivationStage S) const; // returns 0,1,2,... for curve
};
