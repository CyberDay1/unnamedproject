#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EnvEnums.h"
#include "EnvTypes.h"
#include "WeatherSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeatherChanged, FName, RegionID, EWeatherType, Weather);

UCLASS()
class UWeatherSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // Active weather per region (can include global entries with Radius<=0 and RegionID="Global")
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weather")
    TArray<FActiveWeather> ActiveWeather;

    UPROPERTY(BlueprintAssignable) FOnWeatherChanged OnWeatherChanged;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // --- Blueprint API ---
    UFUNCTION(BlueprintCallable, Category="Weather")
    void SetWeather(const FEnvironmentRegion& Region, EWeatherType Type, float DurationSeconds = 0.f);

    UFUNCTION(BlueprintCallable, Category="Weather")
    void ClearWeather(FName RegionID, EWeatherType Type);

    // Special events
    UFUNCTION(BlueprintCallable, Category="Weather|Events")
    void TriggerBloodMoon(float DurationSeconds);

    UFUNCTION(BlueprintCallable, Category="Weather|Events")
    void TriggerBlueMoon(float DurationSeconds);

    UFUNCTION(BlueprintCallable, Category="Weather|Events")
    void TriggerQiFogRain(const FEnvironmentRegion& Region, float DurationSeconds);

    // Query combined modifiers at a world location or region
    UFUNCTION(BlueprintPure, Category="Weather")
    FEnvironmentModifiers GetModifiersAtLocation(const FVector& WorldPos) const;

    UFUNCTION(BlueprintPure, Category="Weather")
    FEnvironmentModifiers GetModifiersForRegion(FName RegionID) const;

private:
    FTimerHandle TickHandle;
    void Tick();

    static bool IsInRegion(const FEnvironmentRegion& R, const FVector& P);
    static FEnvironmentModifiers ModifiersFor(EWeatherType Type);
};
