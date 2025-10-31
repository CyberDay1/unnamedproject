#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Combat/CombatTypes.h"
#include "SkillTypes.generated.h"

UENUM(BlueprintType)
enum class ESkillClass : uint8
{
    SwordMaster,
    Assassin,
    Cleric,
    Mage,
    Monk,
    Wanderer,
    BeastCultivator
};

UENUM(BlueprintType)
enum class ESkillKind : uint8
{
    Active,
    Passive,
    Ultimate
};

UENUM(BlueprintType)
enum class ECastType : uint8
{
    Instant,
    Channeled,
    Charged,
    Buff,
    Area
};

USTRUCT(BlueprintType)
struct FSkillMasteryTuning
{
    GENERATED_BODY()

    // Per-level additive tuning ranges (application selected per skill)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamagePerLevelMin = 0.10f; // +10%

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamagePerLevelMax = 0.15f; // +15%

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float QiCostRedPerLevel = 0.02f; // -2%

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CooldownRedPerLevel = 0.03f; // -3%
};

USTRUCT(BlueprintType)
struct FSkillDefinition : public FTableRowBase
{
    GENERATED_BODY()

    // Identification
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESkillClass ClassReq = ESkillClass::SwordMaster;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESkillKind Kind = ESkillKind::Active;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECastType CastType = ECastType::Instant;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag SkillTag; // e.g., Skill.Sword.QiSlash

    // Requirements & unlocks
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 UnlockMajorRealmStep = 0; // 0=any; ultimates require step % 3 == 0

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer RequiresTags;

    // Base mechanics (percent-based; integrates with CombatComponent scaling stat)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseDamagePercent = 1.5f; // 150% single-hit default

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NumHits = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float QiCostFraction = 0.06f; // 6% Qi

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CooldownSeconds = 8.f; // 5–12 typical

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Range = 240.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius = 75.f;

    // Element and affinity
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EElementType> Element = EElementType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ElementAffinityBonus = 0.10f; // +10% if matching

    // Movement (for movement skills)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DashDistance = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DashInvulnSeconds = 0.2f;

    // Mastery configuration (10 levels)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSkillMasteryTuning Mastery;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bReduceQiWithMastery = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bReduceCooldownWithMastery = true;

    // Ultimate specifics
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsChanneledUltimate = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ChannelSeconds = 0.f; // 1–3 for chan

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float UltimateCooldownSeconds = 600.f; // 10–15 min

    // Presentation hooks
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName NiagaraCastFX = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName NiagaraImpactFX = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* CastSFX = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* ImpactSFX = nullptr;

    // For utility/defense skills
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ShieldAmountPercent = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BuffSeconds = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageReducePercent = 0.f; // e.g., 0.2 = 20% DR
};

USTRUCT(BlueprintType)
struct FSkillProgress
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MasteryLevel = 0; // 0..10

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MasteryXP = 0.f; // 0..1 for next
};

