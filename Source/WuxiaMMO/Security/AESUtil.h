#pragma once
#include "CoreMinimal.h"

struct FAESGCMEncryptResult
{
    TArray<uint8> Ciphertext;
    TArray<uint8> Nonce;   // 12 bytes
    TArray<uint8> Tag;     // 16 bytes
    bool bOk = false;
};

class AESUtil
{
public:
    // Key must be 32 bytes (256-bit). Nonce 12 bytes (will be generated if empty).
    static bool LoadKeyFromEnv(TArray<uint8>& OutKey); // reads WUXIA_AES_KEY_32_BASE64
    static FAESGCMEncryptResult Encrypt(const TArray<uint8>& Plain, const TArray<uint8>& Key, const TArray<uint8>& NonceOpt = TArray<uint8>());
    static bool Decrypt(const TArray<uint8>& Cipher, const TArray<uint8>& Key, const TArray<uint8>& Nonce, const TArray<uint8>& Tag, TArray<uint8>& OutPlain);
};
