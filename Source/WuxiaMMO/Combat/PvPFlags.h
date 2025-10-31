#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PvPFlags.generated.h"

class AActor;

// Simple gate for now; later tie to faction/region/war states
UINTERFACE(BlueprintType)
class UPvPFlagProvider : public UInterface
{
    GENERATED_BODY()
};

class IPvPFlagProvider
{
    GENERATED_BODY()

public:
    virtual bool IsPvPEnabled() const { return false; }
    virtual bool IsSameFactionAs(const AActor* Other) const { return false; }
    virtual bool IsProtectedRegion() const { return false; }
};
