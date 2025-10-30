#pragma once

#include "CoreMinimal.h"

namespace SaveCrypto
{
    bool EncryptToBase64(const TArray<uint8>& Plain, const TArray<uint8>& AESKey, FString& OutBase64);
    bool DecryptFromBase64(const FString& Base64, const TArray<uint8>& AESKey, TArray<uint8>& OutPlain);
}
