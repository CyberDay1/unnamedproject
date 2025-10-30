#pragma once

#include "CoreMinimal.h"

struct FPersistenceConfig
{
    static inline FString SaveUrl()
    {
        return TEXT("https://example.com/api/saveProfile");
    }

    static inline FString LoadUrl()
    {
        return TEXT("https://example.com/api/loadProfile");
    }

    static inline TArray<uint8> DefaultAESKey()
    {
        TArray<uint8> Key;
        Key.AddZeroed(32);
        return Key;
    }

    static constexpr float MinSecondsBetweenSaves = 5.0f;
    static constexpr float MaxSecondsUntilForced = 30.0f;
    static constexpr double CooldownTTLSeconds = 3600.0;
};
