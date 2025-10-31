#pragma once
#include "CoreMinimal.h"
#include "EnvEnums.generated.h"

UENUM(BlueprintType)
enum class EDayPhase : uint8
{
    Day,
    Night
};

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    Sunny,
    Overcast,
    LightRain,
    HeavyRain,
    Thunderstorm,
    Snow,
    Fog,

    QiFogRain,   // +50% passive Qi regen, +25% meditation
    BlueMoon,    // +25% gathering yield
    BloodMoon    // Beast tide: 5x spawns (non-boss), +25% ATK/DEF, extended roam
};

UENUM(BlueprintType)
enum class ESeason : uint8
{
    Spring,
    Summer,
    Autumn,
    Winter
};
