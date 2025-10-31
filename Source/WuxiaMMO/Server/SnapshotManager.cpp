#include "Server/SnapshotManager.h"

#include "HAL/FileManager.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/DateTime.h"
#include "Misc/Paths.h"
#include "Security/AESUtil.h"

FString USnapshotManager::SnapDir;
int32 USnapshotManager::KeepCount = 24;

static bool WriteFileBytes(const FString& Path, const TArray<uint8>& Data)
{
    TUniquePtr<IFileHandle> Handle(FPlatformFileManager::Get().GetPlatformFile().OpenWrite(*Path));
    if (!Handle.IsValid())
    {
        return false;
    }
    const bool bOk = Handle->Write(Data.GetData(), Data.Num());
    return bOk;
}

bool USnapshotManager::Initialize(const FString& Dir, int32 MaxCount)
{
    SnapDir = FPaths::ConvertRelativePathToFull(Dir.IsEmpty() ? TEXT("Saved/Snapshots") : Dir);
    KeepCount = FMath::Max(1, MaxCount);
    return EnsureDir();
}

bool USnapshotManager::EnsureDir()
{
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*SnapDir))
    {
        return PlatformFile.CreateDirectoryTree(*SnapDir);
    }
    return true;
}

void USnapshotManager::PruneOld()
{
    IFileManager& FileManager = IFileManager::Get();
    TArray<FString> Files;
    FileManager.FindFiles(Files, *(FPaths::Combine(SnapDir, TEXT("*.snap"))), true, false);
    if (Files.Num() <= KeepCount)
    {
        return;
    }

    Files.Sort();
    const int32 ToDelete = Files.Num() - KeepCount;
    for (int32 Index = 0; Index < ToDelete; ++Index)
    {
        const FString FullPath = FPaths::Combine(SnapDir, Files[Index]);
        FileManager.Delete(*FullPath, false, true);
    }
}

bool USnapshotManager::WriteSnapshot(const TArray<uint8>& JsonBytes, FSnapshotMeta& OutMeta)
{
    if (!EnsureDir())
    {
        return false;
    }

    TArray<uint8> Key;
    if (!AESUtil::LoadKeyFromEnv(Key))
    {
        return false;
    }

    const FAESGCMEncryptResult Enc = AESUtil::Encrypt(JsonBytes, Key);
    if (!Enc.bOk)
    {
        return false;
    }

    TArray<uint8> Blob;
    Blob.Reserve(Enc.Nonce.Num() + Enc.Tag.Num() + Enc.Ciphertext.Num());
    Blob.Append(Enc.Nonce);
    Blob.Append(Enc.Tag);
    Blob.Append(Enc.Ciphertext);

    const int64 NowUnix = FDateTime::UtcNow().ToUnixTimestamp();
    const FString FileName = FString::Printf(TEXT("%lld.snap"), NowUnix);
    const FString FullPath = FPaths::Combine(SnapDir, FileName);

    if (!WriteFileBytes(FullPath, Blob))
    {
        return false;
    }

    OutMeta.FilePath = FullPath;
    OutMeta.UnixTime = NowUnix;
    OutMeta.SizeBytes = Blob.Num();
    OutMeta.bValid = true;

    PruneOld();
    return true;
}

bool USnapshotManager::FindLatest(FSnapshotMeta& OutMeta)
{
    IFileManager& FileManager = IFileManager::Get();
    TArray<FString> Files;
    FileManager.FindFiles(Files, *(FPaths::Combine(SnapDir, TEXT("*.snap"))), true, false);
    if (Files.Num() == 0)
    {
        return false;
    }

    Files.Sort();
    const FString& LatestName = Files.Last();
    const FString FullPath = FPaths::Combine(SnapDir, LatestName);

    OutMeta.FilePath = FullPath;
    OutMeta.UnixTime = FCString::Atoi64(*FPaths::GetBaseFilename(LatestName));
    OutMeta.SizeBytes = IFileManager::Get().FileSize(*FullPath);
    OutMeta.bValid = OutMeta.SizeBytes > 0;
    return OutMeta.bValid;
}

bool USnapshotManager::ReadLatest(TArray<uint8>& OutJsonBytes)
{
    FSnapshotMeta Meta;
    if (!FindLatest(Meta))
    {
        return false;
    }

    TUniquePtr<IFileHandle> Handle(IFileManager::Get().CreateFileReader(*Meta.FilePath));
    if (!Handle.IsValid())
    {
        return false;
    }

    TArray<uint8> Blob;
    Blob.SetNumUninitialized(Meta.SizeBytes);
    if (!Handle->Read(Blob.GetData(), Blob.Num()))
    {
        return false;
    }

    if (Blob.Num() < 28)
    {
        return false;
    }

    TArray<uint8> Nonce;
    Nonce.Append(Blob.GetData(), 12);

    TArray<uint8> Tag;
    Tag.Append(Blob.GetData() + 12, 16);

    TArray<uint8> Ciphertext;
    Ciphertext.Append(Blob.GetData() + 28, Blob.Num() - 28);

    TArray<uint8> Key;
    if (!AESUtil::LoadKeyFromEnv(Key))
    {
        return false;
    }

    return AESUtil::Decrypt(Ciphertext, Key, Nonce, Tag, OutJsonBytes);
}

