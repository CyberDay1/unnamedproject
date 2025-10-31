#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SessionManagerSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FSessionRecord
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString AccountId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Token;  // opaque
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int64   ExpiresUnix = 0;
};

UCLASS()
class USessionManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Session") bool InitFromConfig();
    UFUNCTION(BlueprintCallable, Category="Session") bool CreateToken(const FString& AccountId, int32 TTLSeconds, FSessionRecord& OutSession);
    UFUNCTION(BlueprintCallable, Category="Session") bool ValidateToken(const FString& Token, FString& OutAccountId);

private:
    FString RedisHost; int32 RedisPort=6379;
    bool RedisSetEX(const FString& Key, const FString& Value, int32 TTL);
    bool RedisGet(const FString& Key, FString& OutValue);
};
