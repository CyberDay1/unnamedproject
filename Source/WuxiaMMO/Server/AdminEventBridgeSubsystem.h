#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AdminEventBridgeSubsystem.generated.h"

UCLASS()
class UAdminEventBridgeSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Admin") bool InitRedisFromConfig();
    UFUNCTION(BlueprintCallable, Category="Admin") void TickPoll(float DeltaSeconds); // call from a manager or bind to tick if desired

private:
    FString RedisHost; int32 RedisPort=6379;
    // TODO: redis pub/sub connection; for now, stub with no-ops
};
