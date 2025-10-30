#pragma once

#include "CoreMinimal.h"
#include "EClassAndElementEnums.h"
#include "CoreMinimal.h"
#include "EClassAndElementEnums.h"
#include "ECultivationStage.h"
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    FString CharacterID; // Server-generated UUID

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    FString FirstName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    FString LastName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    EClassType ClassType = EClassType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    EElementType ElementAffinity = EElementType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    FString Affiliation; // Sects or Powers (Orthodox, Demonic, etc.)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation")
    ECultivationStage CultivationStage = ECultivationStage::MortalBody;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation")
    uint8 SubStage = 1; // 1-9

    FString GetFullName() const
    {
        return FString::Printf(TEXT("%s %s"), *FirstName, *LastName);
    }
};
