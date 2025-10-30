#include "Data/Save/SaveEncryption.h"

#include "Misc/AES.h"
#include "Misc/Base64.h"

namespace
{
    constexpr int32 AESBlockSize = 16;

    void PKCS7Pad(TArray<uint8>& Data)
    {
        int32 Pad = AESBlockSize - (Data.Num() % AESBlockSize);
        if (Pad == 0)
        {
            Pad = AESBlockSize;
        }

        const uint8 PadValue = static_cast<uint8>(Pad);
        const int32 StartIndex = Data.AddUninitialized(Pad);
        for (int32 Index = 0; Index < Pad; ++Index)
        {
            Data[StartIndex + Index] = PadValue;
        }
    }

    bool PKCS7Unpad(TArray<uint8>& Data)
    {
        if (Data.Num() == 0)
        {
            return false;
        }

        const uint8 PadValue = Data.Last();
        if (PadValue == 0 || PadValue > AESBlockSize || PadValue > Data.Num())
        {
            return false;
        }

        for (int32 Index = 0; Index < PadValue; ++Index)
        {
            if (Data[Data.Num() - 1 - Index] != PadValue)
            {
                return false;
            }
        }

        Data.RemoveAt(Data.Num() - PadValue, PadValue);
        return true;
    }
}

bool SaveCrypto::EncryptToBase64(const TArray<uint8>& Plain, const TArray<uint8>& AESKey, FString& OutBase64)
{
    if (AESKey.Num() != 32)
    {
        return false;
    }

    TArray<uint8> Buffer = Plain;
    PKCS7Pad(Buffer);

    TArray<uint8> KeyCopy = AESKey;
    FAES::EncryptData(Buffer.GetData(), Buffer.Num(), KeyCopy.GetData());
    OutBase64 = FBase64::Encode(Buffer);
    return true;
}

bool SaveCrypto::DecryptFromBase64(const FString& Base64, const TArray<uint8>& AESKey, TArray<uint8>& OutPlain)
{
    if (AESKey.Num() != 32)
    {
        return false;
    }

    TArray<uint8> Buffer;
    if (!FBase64::Decode(Base64, Buffer))
    {
        return false;
    }

    TArray<uint8> KeyCopy = AESKey;
    FAES::DecryptData(Buffer.GetData(), Buffer.Num(), KeyCopy.GetData());
    if (!PKCS7Unpad(Buffer))
    {
        return false;
    }

    OutPlain = MoveTemp(Buffer);
    return true;
}
