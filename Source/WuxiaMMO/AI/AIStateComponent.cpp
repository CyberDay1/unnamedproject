#include "AI/AIStateComponent.h"
#include "Environment/WorldTimeSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UAIStateComponent::BeginPlay()
{
    Super::BeginPlay();
    PrimaryComponentTick.bCanEverTick = true;
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        TimeSys = GI->GetSubsystem<UWorldTimeSubsystem>();
    }
}

void UAIStateComponent::TickComponent(float DeltaTime, ELevelTick, FActorComponentTickFunction*)
{
    // Handle sleeping schedule for Humans
    if (Kind == EAIKind::Human && TimeSys.IsValid())
    {
        const float H = TimeSys.Get()->GetHourOfDay();
        const bool bShouldSleep = (H >= Stats.SleepStartHour || H < Stats.WakeHour);
        if (bShouldSleep && CurrentState != EAIState::Sleeping) CurrentState = EAIState::Sleeping;
        else if (!bShouldSleep && CurrentState == EAIState::Sleeping) CurrentState = EAIState::Idle;
    }
}
