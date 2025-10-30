#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WuxiaCharacterAnimInstance.generated.h"

UCLASS()
class UWuxiaCharacterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UWuxiaCharacterAnimInstance();

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(BlueprintReadOnly, Category="Movement")
    float GroundSpeed;
};
