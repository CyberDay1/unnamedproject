#pragma once

#include "CoreMinimal.h"
#include "SyncJobTypes.generated.h"

UENUM(BlueprintType)
enum class ESyncDomain : uint8
{
    Player     UMETA(DisplayName = "Player"),
    World      UMETA(DisplayName = "World"),
    Server     UMETA(DisplayName = "Server")
};

UENUM(BlueprintType)
enum class ESyncSensitivity : uint8
{
    Normal     UMETA(DisplayName = "Normal"),
    Sensitive  UMETA(DisplayName = "Sensitive")
};

USTRUCT(BlueprintType)
struct FSyncJob
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sync")
    ESyncDomain Domain = ESyncDomain::Player;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sync")
    FString Key;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sync")
    FString Topic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sync")
    FString JsonPayload;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sync")
    bool bUpsert = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sync")
    ESyncSensitivity Sensitivity = ESyncSensitivity::Sensitive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sync")
    int64 Version = 0;
};

