#pragma once

#include "CoreMinimal.h"
#include "SnapshotManager.generated.h"

USTRUCT()
struct FSnapshotMeta
{
    GENERATED_BODY()

    FString FilePath;
    int64 UnixTime = 0;
    int64 SizeBytes = 0;
    bool bValid = false;
};

class USnapshotManager
{
public:
    static bool Initialize(const FString& Dir, int32 MaxCount);
    static bool WriteSnapshot(const TArray<uint8>& JsonBytes, FSnapshotMeta& OutMeta);
    static bool FindLatest(FSnapshotMeta& OutMeta);
    static bool ReadLatest(TArray<uint8>& OutJsonBytes);

private:
    static FString SnapDir;
    static int32 KeepCount;

    static bool EnsureDir();
    static void PruneOld();
};

