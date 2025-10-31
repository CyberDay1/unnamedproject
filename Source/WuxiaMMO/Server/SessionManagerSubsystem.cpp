#include "Server/SessionManagerSubsystem.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Guid.h"

// NOTE: These Redis calls are stubs. Replace with hiredis/redis-plus-plus or platform plugin.
bool USessionManagerSubsystem::InitFromConfig()
{
    GConfig->GetString(TEXT("Wuxia.Server"), TEXT("RedisHost"), RedisHost, GGameIni);
    GConfig->GetInt   (TEXT("Wuxia.Server"), TEXT("RedisPort"), RedisPort, GGameIni);
    return !RedisHost.IsEmpty();
}
bool USessionManagerSubsystem::RedisSetEX(const FString& Key, const FString& Value, int32 TTL) { return true; }
bool USessionManagerSubsystem::RedisGet(const FString& Key, FString& OutValue) { OutValue.Empty(); return false; }

bool USessionManagerSubsystem::CreateToken(const FString& AccountId, int32 TTLSeconds, FSessionRecord& OutSession)
{
    const FString Token = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
    const FString Key = FString::Printf(TEXT("sess:%s"), *Token);
    if (!RedisSetEX(Key, AccountId, TTLSeconds)) return false;

    OutSession.AccountId = AccountId;
    OutSession.Token = Token;
    OutSession.ExpiresUnix = FDateTime::UtcNow().ToUnixTimestamp() + TTLSeconds;
    return true;
}

bool USessionManagerSubsystem::ValidateToken(const FString& Token, FString& OutAccountId)
{
    const FString Key = FString::Printf(TEXT("sess:%s"), *Token);
    return RedisGet(Key, OutAccountId); // true if found
}
