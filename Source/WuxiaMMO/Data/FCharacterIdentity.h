#pragma once

#include "CoreMinimal.h"
#include "EClassAndElementEnums.h"
#include "FCharacterIdentity.generated.h"

USTRUCT(BlueprintType)
struct FCharacterIdentity
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Character")
    FString CharacterID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Character")
    FString CharacterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Character")
    EClassType ClassType = EClassType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Character")
    EElementType ElementAffinity = EElementType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category="Character")
    int32 Level = 1;
};
