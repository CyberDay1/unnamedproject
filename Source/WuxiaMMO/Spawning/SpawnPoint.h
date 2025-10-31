#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnTypes.h"
#include "SpawnPoint.generated.h"

// Optional placed actor to register a SpawnGroup via editor
UCLASS()
class ASpawnPoint : public AActor
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning") FSpawnGroup Group;

    UFUNCTION(BlueprintCallable) void Register();
    virtual void BeginPlay() override;
};
