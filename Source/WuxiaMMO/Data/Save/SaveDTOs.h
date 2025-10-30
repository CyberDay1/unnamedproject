#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/FCharacterIdentity.h"
#include "Data/FCharacterStats.h"
#include "Data/FCultivationProgress.h"
#include "Data/FItemDefinition.h"
#include "Data/FEquipmentSlot.h"
#include "Items/ItemInstance.h"
#include "Data/FQuestState.h"
#include "Data/EFaction.h"
#include "Data/Reputation/FSectReputation.h"
#include "Data/Currency/ECurrencyType.h"
#include "Data/FSkillDefinition.h"
#include "SaveDTOs.generated.h"

USTRUCT(BlueprintType)
struct FCooldownSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CooldownID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double StartTime = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Duration = 0.0;
};

USTRUCT(BlueprintType)
struct FEquipmentSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<EEquipmentSlot, FItemInstance> Equipped;
};

USTRUCT(BlueprintType)
struct FSkillSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, int32> SkillLevels;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, float> SkillXP;
};

USTRUCT(BlueprintType)
struct FReputationSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<EFaction, float> FactionReputation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSectReputation> SectReputations;
};

USTRUCT(BlueprintType)
struct FCurrencySnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<ECurrencyType, int64> Balances;
};

USTRUCT(BlueprintType)
struct FPlayerProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString AccountID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CharacterID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCharacterIdentity Identity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCharacterStats Stats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCultivationProgress Cultivation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSkillSnapshot Skills;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FItemInstance> InventoryItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FEquipmentSnapshot Equipment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FQuestState> ActiveQuests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FReputationSnapshot Reputation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCurrencySnapshot Currency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FCooldownSnapshot> Cooldowns;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SchemaVersion = 1;
};
