#include "Characters/WuxiaCharacterAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"

UWuxiaCharacterAnimInstance::UWuxiaCharacterAnimInstance()
    : GroundSpeed(0.f)
{
}

void UWuxiaCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
}

void UWuxiaCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* OwningPawn = TryGetPawnOwner();
    if (!OwningPawn)
    {
        GroundSpeed = 0.f;
        return;
    }

    const FVector Velocity = OwningPawn->GetVelocity();
    GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
}
