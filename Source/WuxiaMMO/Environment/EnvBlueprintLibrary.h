#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EnvBlueprintLibrary.generated.h"

class ADirectionalLight;
class ASkyLight;

UCLASS()
class UEnvBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    // Rotate sun based on hour [0..24)
    UFUNCTION(BlueprintCallable, Category="Environment")
    static void ApplySunAngle(ADirectionalLight* Sun, float HourOfDay);

    // Simple skylight refresh (call occasionally)
    UFUNCTION(BlueprintCallable, Category="Environment")
    static void RefreshSkyLight(ASkyLight* Sky);
};
