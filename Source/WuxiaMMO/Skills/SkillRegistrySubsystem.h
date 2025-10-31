#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SkillRegistrySubsystem.generated.h"

class UDataTable;

UCLASS()
class USkillRegistrySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UDataTable* SkillTable = nullptr;

    UFUNCTION(BlueprintPure)
    UDataTable* GetSkillTable() const { return SkillTable; }
};

