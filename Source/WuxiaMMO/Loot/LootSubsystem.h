#pragma once

#include "CoreMinimal.h"
#include "Loot/LootTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimerManager.h"
#include "LootSubsystem.generated.h"

class UDataTable;
class UWeatherSubsystem;
class UPartySubsystem;
class UAuctionHouseSubsystem;
class USpatialRingComponent;
class UQuestComponent;
class UActionSaveRouterComponent;
class AActor;
struct FItemInstance;
struct FPartyInfo;

USTRUCT()
struct FLootResult
{
    GENERATED_BODY()

    FName ItemID = NAME_None;
    int32 Quantity = 0;
    ELootRarity Rarity = ELootRarity::Common;
};

USTRUCT()
struct FLootSession
{
    GENERATED_BODY()

    FString SessionID;
    FString PartyID;
    FString MasterAccountID;
    FString KillerCharacterID;
    TArray<FLootResult> NonTrash;
    TArray<FLootResult> Trash;
    double ExpireAtWorldSeconds = 0.0;
};

UCLASS()
class ULootSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    UDataTable* LootTable = nullptr;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Loot")
    void HandleDeathLoot(AActor* DeadActor, AActor* KillerActor, FName DropTableID, int32 KillerLuckStat);

    UFUNCTION(BlueprintCallable, Category = "Loot|Master")
    bool Master_AssignTo(const FString& SessionID, const FString& TargetCharacterID, int32 LootIndex);

    UFUNCTION(BlueprintCallable, Category = "Loot|Master")
    bool Master_StartRoll(const FString& SessionID, int32 LootIndex);

    UFUNCTION(BlueprintCallable, Category = "Loot|Master")
    bool Master_ListOnAuction(const FString& SessionID, int32 LootIndex, int64 OptionalBuyout);

    UFUNCTION(BlueprintCallable, Category = "Loot|Roll")
    bool Player_SubmitRoll(const FString& SessionID, const FString& CharacterID, int32 Roll);

    int32 GetLuckStatForActor(const AActor* Actor) const;

private:
    void Tick();

    FLootSession& CreateSession(const FString& PartyID, const FString& MasterID, const FString& KillerCharID);
    void ResolveRoundRobinTrash(FLootSession& Session, FPartyInfo& PartyInfo);
    void AutoListExpiredSessions();
    void EvaluateOpenRolls(const FString& SessionID, FLootSession& Session);

    TArray<FLootResult> GenerateLoot(FName DropTableID, int32 LuckStat) const;
    static int32 RandomQuantity(int32 MinQ, int32 MaxQ);
    float BlueMoonQuantityMult() const;
    float BloodMoonRarityBonus() const;

    bool GrantLootToActor(AActor* TargetActor, const FLootResult& Loot) const;
    bool GrantLootToCharacter(const FString& CharacterID, const FLootResult& Loot, const FPartyInfo& PartyInfo);
    void NotifyQuestProgress(AActor* TargetActor, const FLootResult& Loot) const;

    FString ResolveCharacterId(const AActor* Actor) const;
    FString ResolveAccountId(const AActor* Actor) const;
    FName ResolveDropTableId(const AActor* Actor) const;

    void RemoveSessionIfEmpty(const FString& SessionID, FLootSession& Session);
    void FinalizeRollWinner(const FString& SessionID, int32 LootIndex, FLootSession& Session, TMap<FString, int32>& Rolls, const FPartyInfo& PartyInfo);

    static EItemRarity ToItemRarity(ELootRarity Rarity);
    FItemInstance BuildItemInstance(const FLootResult& Result) const;

private:
    FTimerHandle TickHandle;

    TMap<FString, FLootSession> Sessions;
    TMap<FString, TMap<int32, TMap<FString, int32>>> PendingRolls;

    TWeakObjectPtr<UWeatherSubsystem> Weather;
    TWeakObjectPtr<UPartySubsystem> Party;
    TWeakObjectPtr<UAuctionHouseSubsystem> Auction;
};
