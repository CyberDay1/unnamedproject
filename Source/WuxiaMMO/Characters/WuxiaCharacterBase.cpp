#include "Characters/WuxiaCharacterBase.h"

#include "Characters/CharacterInitializerComponent.h"

AWuxiaCharacterBase::AWuxiaCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;
    Initializer = CreateDefaultSubobject<UCharacterInitializerComponent>(TEXT("Initializer"));
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

void AWuxiaCharacterBase::InitializeFromData(const FCharacterIdentity& Identity, const FCharacterStats& Stats)
{
    CharacterIdentity = Identity;
    CharacterStats = Stats;

    Health = Stats.Health;
    Qi = Stats.Qi;
    Stamina = Stats.Stamina;

    if (Initializer)
    {
        Initializer->InitializeCharacter(Identity, Stats);
    }
}

void AWuxiaCharacterBase::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector(), Value);
}

void AWuxiaCharacterBase::MoveRight(float Value)
{
    AddMovementInput(GetActorRightVector(), Value);
}
