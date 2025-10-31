#include "Security/AESUtil.h"
#include "Misc/Guid.h"
#include "Misc/ScopeLock.h"
#include "Misc/Base64.h"

// NOTE: This uses platform crypto backends if available; for skeleton, we simulate with XOR fallback in editor-only to avoid external deps.
// Replace with a proper AES-GCM implementation (OpenSSL) in production builds.

static void RandomBytes(TArray<uint8>& Out, int32 Len)
{
    Out.SetNumUninitialized(Len);
    for (int32 i=0;i<Len;++i) { Out[i] = (uint8)FMath::RandRange(0,255); }
}

bool AESUtil::LoadKeyFromEnv(TArray<uint8>& OutKey)
{
    FString B64 = FPlatformMisc::GetEnvironmentVariable(TEXT("WUXIA_AES_KEY_32_BASE64"));
    if (B64.IsEmpty()) return false;
    TArray<uint8> Raw;
    if (!FBase64::Decode(B64, Raw)) return false;
    if (Raw.Num() != 32) return false;
    OutKey = MoveTemp(Raw);
    return true;
}

FAESGCMEncryptResult AESUtil::Encrypt(const TArray<uint8>& Plain, const TArray<uint8>& Key, const TArray<uint8>& NonceOpt)
{
    FAESGCMEncryptResult R;
    if (Key.Num() != 32) return R;

    TArray<uint8> Nonce = NonceOpt;
    if (Nonce.Num() == 0) RandomBytes(Nonce, 12);
    R.Nonce = Nonce;

    // EDITOR-ONLY PLACEHOLDER: XOR (DO NOT USE IN PROD). Replace with AES-GCM.
    R.Ciphertext = Plain;
#if WITH_EDITOR
    for (int32 i=0;i<R.Ciphertext.Num();++i) R.Ciphertext[i] ^= Key[i % Key.Num()];
    R.Tag.SetNumZeroed(16);
    R.bOk = true;
#else
    // TODO: Implement real AES-GCM with platform crypto or OpenSSL.
    R.bOk = false;
#endif
    return R;
}

bool AESUtil::Decrypt(const TArray<uint8>& Cipher, const TArray<uint8>& Key, const TArray<uint8>& Nonce, const TArray<uint8>& Tag, TArray<uint8>& OutPlain)
{
    if (Key.Num() != 32 || Nonce.Num() != 12 || Tag.Num() != 16) return false;
#if WITH_EDITOR
    OutPlain = Cipher;
    for (int32 i=0;i<OutPlain.Num();++i) OutPlain[i] ^= Key[i % Key.Num()];
    return true;
#else
    // TODO: Implement real AES-GCM.
    return false;
#endif
}
