#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WuxiaGameSessionSubsystem.generated.h"

UCLASS()
class UWuxiaGameSessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UWuxiaGameSessionSubsystem();

    UFUNCTION(BlueprintCallable, Category="Session")
    void InitializeSession();
};
