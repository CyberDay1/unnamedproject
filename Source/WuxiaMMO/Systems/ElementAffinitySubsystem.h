#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FElementAffinity.h"
#include "ElementAffinitySubsystem.generated.h"

UCLASS()
class UElementAffinitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Affinity")
    bool CanChangeAffinity(const FString& CharacterID) const;

    UFUNCTION(BlueprintCallable, Category="Affinity")
    void RequestAffinityChange(const FString& CharacterID, EElementType NewElement);

    UFUNCTION(BlueprintCallable, Category="Affinity")
    float GetRemainingCooldown(const FString& CharacterID) const;

private:
    UPROPERTY()
    TMap<FString, double> LastChangeTimestamps;

    UPROPERTY(EditAnywhere, Category="Affinity|Config")
    double CooldownSeconds = 604800; // 7 real days

    UPROPERTY(EditAnywhere, Category="Affinity|Config")
    float QiOfferingCost = 500.0f; // Example value
};
