#include "Server/AdminEventBridgeSubsystem.h"
#include "Misc/ConfigCacheIni.h"

bool UAdminEventBridgeSubsystem::InitRedisFromConfig()
{
    GConfig->GetString(TEXT("Wuxia.Server"), TEXT("RedisHost"), RedisHost, GGameIni);
    GConfig->GetInt   (TEXT("Wuxia.Server"), TEXT("RedisPort"), RedisPort, GGameIni);
    return !RedisHost.IsEmpty();
}

void UAdminEventBridgeSubsystem::TickPoll(float DeltaSeconds)
{
    // TODO: subscribe to "wuxia_events" channel; parse JSON, e.g., {"event":"BloodMoon","region_id":"X","duration":600}
    // Call gameplay systems to toggle weather/event actors.
}
