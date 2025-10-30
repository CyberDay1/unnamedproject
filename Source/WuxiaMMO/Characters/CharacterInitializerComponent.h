#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/FCharacterIdentity.h"
#include "Data/FCharacterStats.h"
#include "Data/EClassAndElementEnums.h"
#include "CharacterInitializerComponent.generated.h"

class ACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UCharacterInitializerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterInitializerComponent();

    UFUNCTION(BlueprintCallable, Category="Character|Init")
    void InitializeCharacter(const FCharacterIdentity& Identity, const FCharacterStats& Stats);

protected:
    virtual void BeginPlay() override;

private:
    void ApplyStats(const FCharacterStats& Stats);
    void ApplyClassDefinition(EClassType ClassType);
    void ApplyElementAffinity(EElementType ElementAffinity);

    UPROPERTY()
    ACharacter* OwnerCharacter;
};
