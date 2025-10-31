#include "Environment/WeatherRegionActor.h"

#include "Buffs/BuffComponent.h"
#include "Components/BoxComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

AWeatherRegionActor::AWeatherRegionActor()
{
    PrimaryActorTick.bCanEverTick = false;

    Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("RegionBox"));
    RootComponent = Volume;
    if (Volume)
    {
        Volume->SetCollisionProfileName(TEXT("OverlapAll"));
        Volume->SetGenerateOverlapEvents(true);
    }
}

void AWeatherRegionActor::BeginPlay()
{
    Super::BeginPlay();

    OnActorBeginOverlap.AddDynamic(this, &AWeatherRegionActor::OnBeginOverlap);
    OnActorEndOverlap.AddDynamic(this, &AWeatherRegionActor::OnEndOverlap);
}

void AWeatherRegionActor::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!OtherActor || BuffOnEnter.IsNone())
    {
        return;
    }

    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
    const TWeakObjectPtr<AActor> Key(OtherActor);

    if (const float* LastTime = LastAppliedTime.Find(Key))
    {
        if ((Now - *LastTime) < ApplyDelaySeconds)
        {
            return;
        }
    }

    if (UBuffComponent* Buffs = OtherActor->FindComponentByClass<UBuffComponent>())
    {
        if (Buffs->ApplyBuffByID(BuffOnEnter, 1, -1.f))
        {
            LastAppliedTime.Add(Key, Now);
        }
    }
}

void AWeatherRegionActor::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!OtherActor)
    {
        return;
    }

    const TWeakObjectPtr<AActor> Key(OtherActor);
    LastAppliedTime.Remove(Key);

    if (BuffOnExitRemoveTag.IsNone())
    {
        return;
    }

    if (UBuffComponent* Buffs = OtherActor->FindComponentByClass<UBuffComponent>())
    {
        const FGameplayTag BuffTag = FGameplayTag::RequestGameplayTag(BuffOnExitRemoveTag, false);
        if (BuffTag.IsValid())
        {
            Buffs->RemoveBuffByTag(BuffTag);
        }
    }
}
