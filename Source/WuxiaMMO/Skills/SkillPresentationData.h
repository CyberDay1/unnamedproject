#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillTypes.h"
#include "SkillPresentationData.generated.h"

class UNiagaraSystem;
class USoundBase;

UCLASS(BlueprintType)
class USkillPresentationData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    FName SkillID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    FSkillDefinition Definition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Presentation")
    TSoftObjectPtr<UNiagaraSystem> CastEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Presentation")
    TSoftObjectPtr<UNiagaraSystem> ImpactEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Presentation")
    TSoftObjectPtr<USoundBase> CastSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Presentation")
    TSoftObjectPtr<USoundBase> ImpactSound;

    UFUNCTION(BlueprintCallable, Category="Skill")
    void PlayCastFX(AActor* Caster) const;

    UFUNCTION(BlueprintCallable, Category="Skill")
    void PlayImpactFX(AActor* Target) const;
};

