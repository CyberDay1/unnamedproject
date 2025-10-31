#include "Skills/SkillPresentationData.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"

namespace
{
    template <typename T>
    T* ResolveSoft(const TSoftObjectPtr<T>& Ptr)
    {
        return Ptr.IsValid() ? Ptr.Get() : Ptr.IsNull() ? nullptr : Ptr.LoadSynchronous();
    }
}

void USkillPresentationData::PlayCastFX(AActor* Caster) const
{
    if (!Caster)
    {
        return;
    }

    if (UNiagaraSystem* System = ResolveSoft(CastEffect))
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(Caster, System, Caster->GetActorLocation());
    }

    if (USoundBase* Sound = ResolveSoft(CastSound))
    {
        UGameplayStatics::PlaySoundAtLocation(Caster, Sound, Caster->GetActorLocation());
    }
}

void USkillPresentationData::PlayImpactFX(AActor* Target) const
{
    if (!Target)
    {
        return;
    }

    if (UNiagaraSystem* System = ResolveSoft(ImpactEffect))
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(Target, System, Target->GetActorLocation());
    }

    if (USoundBase* Sound = ResolveSoft(ImpactSound))
    {
        UGameplayStatics::PlaySoundAtLocation(Target, Sound, Target->GetActorLocation());
    }
}

