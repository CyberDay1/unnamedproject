#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeatherRegionActor.generated.h"

class UBoxComponent;
class UBuffComponent;

UENUM(BlueprintType)
enum class EWeatherKind : uint8
{
    Clear,
    QiRain,
    BlueMoon,
    BloodMoon,
    Storm,
    Fog
};

UCLASS()
class AWeatherRegionActor : public AActor
{
    GENERATED_BODY()
public:
    AWeatherRegionActor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather") EWeatherKind Weather = EWeatherKind::Clear;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather") float ApplyDelaySeconds = 0.1f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather") FName BuffOnEnter = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather") FName BuffOnExitRemoveTag = NAME_None;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weather", meta=(AllowPrivateAccess="true"))
    UBoxComponent* Volume;

    UFUNCTION()
    void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

    UFUNCTION()
    void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

    TMap<TWeakObjectPtr<AActor>, float> LastAppliedTime;
};
