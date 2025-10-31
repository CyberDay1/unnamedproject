#pragma once

#include "CoreMinimal.h"
#include "Combat/DamageTypes.h"
#include "UObject/Interface.h"
#include "StatInterfaces.generated.h"

// Minimal stat getters expected on Source/Target; swap for your real components later
UINTERFACE(BlueprintType)
class UCombatStatProvider : public UInterface
{
    GENERATED_BODY()
};

class ICombatStatProvider
{
    GENERATED_BODY()

public:
    // OFFENSE
    virtual float GetAttackPower() const { return 100.f; }
    virtual float GetCritChance() const { return 0.10f; }
    virtual float GetCritDamage() const { return 1.50f; }
    virtual float GetHitChance() const { return 0.90f; }

    // DEFENSE
    virtual float GetArmor() const { return 50.f; }
    virtual float GetDodge() const { return 0.05f; }
    virtual float GetResist(EElementType) const { return 0.f; }

    // LUCK hook
    virtual float GetLuck() const { return 0.f; }
};
