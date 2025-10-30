#pragma once

#include "CoreMinimal.h"
#include "EClassAndElementEnums.h"
#include "ECultivationStage.h"
#include "FCharacterIdentity.generated.h"

UENUM(BlueprintType)
enum class ESex : uint8
{
    Male      UMETA(DisplayName="Male"),
    Female    UMETA(DisplayName="Female"),
    Unspecified UMETA(DisplayName="Unspecified")
};

USTRUCT(BlueprintType)
struct FCharacterIdentity
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    FString CharacterID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    FString FirstName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    FString LastName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    ESex Sex = ESex::Unspecified;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    EClassType ClassType = EClassType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    EElementType ElementAffinity = EElementType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    FString Affiliation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation")
    ECultivationStage CultivationStage = ECultivationStage::MortalBody;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cultivation")
    uint8 SubStage = 1;

    FString GetFullName() const
    {
        return FString::Printf(TEXT("%s %s"), *FirstName, *LastName);
    }
};
