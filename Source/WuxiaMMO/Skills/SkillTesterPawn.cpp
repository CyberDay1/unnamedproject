#include "Skills/SkillTesterPawn.h"
#include "Skills/SkillRuntimeComponent.h"
#include "Skills/SkillMasteryComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"

ASkillTesterPawn::ASkillTesterPawn()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    SkillComponent = CreateDefaultSubobject<USkillRuntimeComponent>(TEXT("SkillRuntime"));
    MasteryComponent = CreateDefaultSubobject<USkillMasteryComponent>(TEXT("SkillMastery"));

    AutoLearnSkillIDs = {
        TEXT("QiSlash"),
        TEXT("CrescentWave"),
        TEXT("FlowGuard"),
        TEXT("FlashStep"),
        TEXT("HeavenSplittingSlash"),
        TEXT("SwordDomain")
    };
}

void ASkillTesterPawn::BeginPlay()
{
    Super::BeginPlay();

    if (SkillComponent)
    {
        for (const FName& SkillID : AutoLearnSkillIDs)
        {
            SkillComponent->LearnSkill(SkillID);
        }
    }

    RefreshHotbarFromSkills();
}

void ASkillTesterPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (!PlayerInputComponent)
    {
        return;
    }

    PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &ASkillTesterPawn::UseSlot1);
    PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ASkillTesterPawn::UseSlot2);
    PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ASkillTesterPawn::UseSlot3);
    PlayerInputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ASkillTesterPawn::UseSlot4);
    PlayerInputComponent->BindKey(EKeys::Five, IE_Pressed, this, &ASkillTesterPawn::UseSlot5);
    PlayerInputComponent->BindKey(EKeys::Six, IE_Pressed, this, &ASkillTesterPawn::UseSlot6);
}

void ASkillTesterPawn::RefreshHotbarFromSkills()
{
    if (SkillComponent)
    {
        SkillComponent->GetKnownSkills(HotbarSkillIDs);
    }
    else
    {
        HotbarSkillIDs.Reset();
    }
}

void ASkillTesterPawn::UseSlot(int32 Index)
{
    if (!SkillComponent)
    {
        return;
    }

    if (!HotbarSkillIDs.IsValidIndex(Index))
    {
        return;
    }

    SkillComponent->UseSkillByID(HotbarSkillIDs[Index]);
}

void ASkillTesterPawn::UseSlot1()
{
    UseSlot(0);
}

void ASkillTesterPawn::UseSlot2()
{
    UseSlot(1);
}

void ASkillTesterPawn::UseSlot3()
{
    UseSlot(2);
}

void ASkillTesterPawn::UseSlot4()
{
    UseSlot(3);
}

void ASkillTesterPawn::UseSlot5()
{
    UseSlot(4);
}

void ASkillTesterPawn::UseSlot6()
{
    UseSlot(5);
}

