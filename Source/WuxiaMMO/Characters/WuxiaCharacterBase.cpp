#include "Characters/WuxiaCharacterBase.h"

AWuxiaCharacterBase::AWuxiaCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWuxiaCharacterBase::BeginPlay()
{
    Super::BeginPlay();
}

void AWuxiaCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWuxiaCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AWuxiaCharacterBase::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector(), Value);
}

void AWuxiaCharacterBase::MoveRight(float Value)
{
    AddMovementInput(GetActorRightVector(), Value);
}
