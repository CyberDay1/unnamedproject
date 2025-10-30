#pragma once
#include "CoreMinimal.h"
#include "FDialogueCondition.h"
#include "FDialogueResponse.generated.h"

USTRUCT(BlueprintType)
struct FDialogueResponse
{
    GENERATED_BODY()

    // Player text
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    FString PlayerLine;

    // Next dialogue node UUID (DataTable lookup)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    FString NextNodeUUID;

    // Optional conditions to show this response
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
    TArray<FDialogueCondition> Conditions;
};
