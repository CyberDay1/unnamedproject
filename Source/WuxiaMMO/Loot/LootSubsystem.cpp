#include "Loot/LootSubsystem.h"

#include "Economy/AuctionHouseSubsystem.h"
#include "Environment/WeatherSubsystem.h"
#include "Items/ItemInstance.h"
#include "Data/FItemDefinition.h"
#include "Party/PartySubsystem.h"
#include "Components/QuestComponent.h"
#include "Components/SpatialRingComponent.h"
#include "Systems/ActionSaveRouterComponent.h"

#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameInstance.h"
#include "Misc/Guid.h"
#include "TimerManager.h"
#include "UObject/UnrealType.h"

namespace
{
    constexpr float LuckAbsolutePerPoint = 0.005f; // +0.5% absolute chance per point
}

void ULootSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        Weather = GameInstance->GetSubsystem<UWeatherSubsystem>();
        Party = GameInstance->GetSubsystem<UPartySubsystem>();
        Auction = GameInstance->GetSubsystem<UAuctionHouseSubsystem>();
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(TickHandle, this, &ULootSubsystem::Tick, 5.0f, true);
    }
}

void ULootSubsystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TickHandle);
    }

    Sessions.Empty();
    PendingRolls.Empty();
    Super::Deinitialize();
}

void ULootSubsystem::Tick()
{
    AutoListExpiredSessions();
}

int32 ULootSubsystem::GetLuckStatForActor(const AActor* Actor) const
{
    if (!Actor)
    {
        return 0;
    }

    if (const FProperty* StatsProp = Actor->GetClass()->FindPropertyByName(TEXT("CharacterStats")))
    {
        if (const FStructProperty* StructProp = CastField<const FStructProperty>(StatsProp))
        {
            const void* StructPtr = StatsProp->ContainerPtrToValuePtr<void>(Actor);
            if (StructPtr)
            {
                if (const FProperty* LuckProp = StructProp->Struct->FindPropertyByName(TEXT("Luck")))
                {
                    if (const FIntProperty* IntProp = CastField<const FIntProperty>(LuckProp))
                    {
                        return IntProp->GetPropertyValue_InContainer(StructPtr);
                    }
                }
            }
        }
    }

    return 0;
}

void ULootSubsystem::HandleDeathLoot(AActor* DeadActor, AActor* KillerActor, FName DropTableID, int32 KillerLuckStat)
{
    if (!DeadActor || !LootTable)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World || World->IsNetMode(NM_Client))
    {
        return;
    }

    if (DropTableID.IsNone())
    {
        DropTableID = ResolveDropTableId(DeadActor);
    }

    if (DropTableID.IsNone())
    {
        return;
    }

    KillerLuckStat = FMath::Max(0, KillerLuckStat);

    FString KillerCharacterID = ResolveCharacterId(KillerActor);
    FString KillerAccountID = ResolveAccountId(KillerActor);

    const TArray<FLootResult> GeneratedLoot = GenerateLoot(DropTableID, KillerLuckStat);
    if (GeneratedLoot.IsEmpty())
    {
        return;
    }

    FPartyInfo PartyInfo;
    const bool bHasParty = !KillerCharacterID.IsEmpty() && Party.IsValid() && Party.Get()->GetPartyByCharacter(KillerCharacterID, PartyInfo) && PartyInfo.Members.Num() > 1;

    if (!bHasParty)
    {
        for (const FLootResult& Result : GeneratedLoot)
        {
            GrantLootToActor(KillerActor, Result);
        }
        return;
    }

    const FString MasterID = PartyInfo.MasterAccountID.IsEmpty() ? KillerAccountID : PartyInfo.MasterAccountID;
    FLootSession& Session = CreateSession(PartyInfo.PartyID, MasterID, KillerCharacterID);

    for (const FLootResult& Result : GeneratedLoot)
    {
        if (Result.Rarity == ELootRarity::Trash)
        {
            Session.Trash.Add(Result);
        }
        else
        {
            Session.NonTrash.Add(Result);
        }
    }

    if (!Session.Trash.IsEmpty())
    {
        ResolveRoundRobinTrash(Session, PartyInfo);
        Party->UpdatePartyState(PartyInfo);
    }

    EvaluateOpenRolls(Session.SessionID, Session);
}

bool ULootSubsystem::Master_AssignTo(const FString& SessionID, const FString& TargetCharacterID, int32 LootIndex)
{
    if (TargetCharacterID.IsEmpty())
    {
        return false;
    }

    FLootSession* Session = Sessions.Find(SessionID);
    if (!Session || !Session->NonTrash.IsValidIndex(LootIndex))
    {
        return false;
    }

    if (!Party.IsValid())
    {
        return false;
    }

    FPartyInfo PartyInfo;
    if (!Party.Get()->GetPartyByID(Session->PartyID, PartyInfo))
    {
        return false;
    }

    if (!GrantLootToCharacter(TargetCharacterID, Session->NonTrash[LootIndex], PartyInfo))
    {
        return false;
    }

    Session->NonTrash.RemoveAt(LootIndex);

    if (TMap<int32, TMap<FString, int32>>* Rolls = PendingRolls.Find(SessionID))
    {
        Rolls->Remove(LootIndex);
        if (Rolls->IsEmpty())
        {
            PendingRolls.Remove(SessionID);
        }
    }

    RemoveSessionIfEmpty(SessionID, *Session);
    Party->UpdatePartyState(PartyInfo);
    return true;
}

bool ULootSubsystem::Master_StartRoll(const FString& SessionID, int32 LootIndex)
{
    FLootSession* Session = Sessions.Find(SessionID);
    if (!Session || !Session->NonTrash.IsValidIndex(LootIndex))
    {
        return false;
    }

    TMap<int32, TMap<FString, int32>>& SessionRolls = PendingRolls.FindOrAdd(SessionID);
    SessionRolls.FindOrAdd(LootIndex).Empty();
    return true;
}

bool ULootSubsystem::Master_ListOnAuction(const FString& SessionID, int32 LootIndex, int64 OptionalBuyout)
{
    if (!Auction.IsValid())
    {
        return false;
    }

    FLootSession* Session = Sessions.Find(SessionID);
    if (!Session || !Session->NonTrash.IsValidIndex(LootIndex))
    {
        return false;
    }

    if (!Party.IsValid())
    {
        return false;
    }

    FPartyInfo PartyInfo;
    if (!Party.Get()->GetPartyByID(Session->PartyID, PartyInfo))
    {
        return false;
    }

    TArray<FString> Beneficiaries;
    for (const FPartyMember& Member : PartyInfo.Members)
    {
        if (!Member.CharacterID.IsEmpty())
        {
            Beneficiaries.Add(Member.CharacterID);
        }
    }

    const FLootResult Loot = Session->NonTrash[LootIndex];

    FAuctionListing Listing;
    Listing.BeneficiaryCharacterIDs = Beneficiaries;
    Listing.ItemID = Loot.ItemID;
    Listing.Quantity = Loot.Quantity;
    Listing.Rarity = Loot.Rarity;
    Listing.BuyoutPrice = OptionalBuyout;

    Auction.Get()->CreateListing(Listing);

    Session->NonTrash.RemoveAt(LootIndex);

    if (TMap<int32, TMap<FString, int32>>* Rolls = PendingRolls.Find(SessionID))
    {
        Rolls->Remove(LootIndex);
        if (Rolls->IsEmpty())
        {
            PendingRolls.Remove(SessionID);
        }
    }

    RemoveSessionIfEmpty(SessionID, *Session);
    return true;
}

bool ULootSubsystem::Player_SubmitRoll(const FString& SessionID, const FString& CharacterID, int32 Roll)
{
    if (Roll < 1 || Roll > 100 || CharacterID.IsEmpty())
    {
        return false;
    }

    FLootSession* Session = Sessions.Find(SessionID);
    if (!Session)
    {
        return false;
    }

    TMap<int32, TMap<FString, int32>>* SessionRolls = PendingRolls.Find(SessionID);
    if (!SessionRolls)
    {
        return false;
    }

    if (!Party.IsValid())
    {
        return false;
    }

    FPartyInfo PartyInfo;
    if (!Party.Get()->GetPartyByID(Session->PartyID, PartyInfo) || PartyInfo.Members.IsEmpty())
    {
        return false;
    }

    bool bAccepted = false;
    for (auto& Pair : *SessionRolls)
    {
        if (!Session->NonTrash.IsValidIndex(Pair.Key))
        {
            continue;
        }

        TMap<FString, int32>& Rolls = Pair.Value;
        Rolls.FindOrAdd(CharacterID) = Roll;
        bAccepted = true;

        if (Rolls.Num() >= PartyInfo.Members.Num())
        {
            FinalizeRollWinner(SessionID, Pair.Key, *Session, Rolls, PartyInfo);
        }
    }

    if (bAccepted)
    {
        Party->UpdatePartyState(PartyInfo);
    }

    return bAccepted;
}

TArray<FLootResult> ULootSubsystem::GenerateLoot(FName DropTableID, int32 LuckStat) const
{
    TArray<FLootResult> Results;

    if (!LootTable)
    {
        return Results;
    }

    const FLootGroup* LootGroup = LootTable->FindRow<FLootGroup>(DropTableID, TEXT("LootSubsystem"));
    if (!LootGroup)
    {
        return Results;
    }

    const float QuantityMultiplier = BlueMoonQuantityMult();
    const float RarityWeightBonus = BloodMoonRarityBonus();
    const float LuckBonus = FMath::Clamp(LuckStat * LuckAbsolutePerPoint, 0.0f, 1.0f);

    for (const FLootItem& Item : LootGroup->Independent)
    {
        if (Item.ItemID.IsNone())
        {
            continue;
        }

        const float Chance = FMath::Clamp(Item.BaseChance + LuckBonus, 0.0f, 1.0f);
        if (FMath::FRand() > Chance)
        {
            continue;
        }

        const int32 BaseQty = RandomQuantity(Item.MinQty, Item.MaxQty);
        const int32 FinalQty = FMath::Max(1, FMath::RoundToInt(static_cast<float>(BaseQty) * QuantityMultiplier));

        FLootResult Result;
        Result.ItemID = Item.ItemID;
        Result.Quantity = FinalQty;
        Result.Rarity = Item.Rarity;
        Results.Add(Result);
    }

    if (LootGroup->PickFromWeighted > 0 && LootGroup->Weighted.Num() > 0)
    {
        const int32 Picks = FMath::Clamp(LootGroup->PickFromWeighted, 1, LootGroup->Weighted.Num());
        TArray<float> Weights;
        Weights.Reserve(LootGroup->Weighted.Num());

        float WeightSum = 0.0f;
        for (const FLootItem& Item : LootGroup->Weighted)
        {
            float Weight = Item.Weight;
            if (RarityWeightBonus > 0.0f && Item.Rarity >= ELootRarity::Rare)
            {
                Weight *= (1.0f + RarityWeightBonus);
            }

            Weights.Add(FMath::Max(0.0f, Weight));
            WeightSum += Weights.Last();
        }

        if (WeightSum <= KINDA_SMALL_NUMBER)
        {
            WeightSum = static_cast<float>(LootGroup->Weighted.Num());
            for (float& Weight : Weights)
            {
                Weight = 1.0f;
            }
        }

        for (int32 PickIndex = 0; PickIndex < Picks; ++PickIndex)
        {
            float Sample = FMath::FRand() * WeightSum;
            int32 ChosenIndex = 0;
            for (; ChosenIndex < Weights.Num(); ++ChosenIndex)
            {
                Sample -= Weights[ChosenIndex];
                if (Sample <= 0.0f)
                {
                    break;
                }
            }

            ChosenIndex = FMath::Clamp(ChosenIndex, 0, LootGroup->Weighted.Num() - 1);
            const FLootItem& Item = LootGroup->Weighted[ChosenIndex];
            if (Item.ItemID.IsNone())
            {
                continue;
            }

            const float Chance = FMath::Clamp(Item.BaseChance + LuckBonus, 0.0f, 1.0f);
            if (FMath::FRand() > Chance)
            {
                continue;
            }

            const int32 BaseQty = RandomQuantity(Item.MinQty, Item.MaxQty);
            const int32 FinalQty = FMath::Max(1, FMath::RoundToInt(static_cast<float>(BaseQty) * QuantityMultiplier));

            FLootResult Result;
            Result.ItemID = Item.ItemID;
            Result.Quantity = FinalQty;
            Result.Rarity = Item.Rarity;
            Results.Add(Result);
        }
    }

    return Results;
}

int32 ULootSubsystem::RandomQuantity(int32 MinQ, int32 MaxQ)
{
    if (MaxQ < MinQ)
    {
        MaxQ = MinQ;
    }
    return FMath::RandRange(MinQ, MaxQ);
}

float ULootSubsystem::BlueMoonQuantityMult() const
{
    if (!Weather.IsValid())
    {
        return 1.0f;
    }

    const FEnvironmentModifiers Mods = Weather.Get()->GetModifiersForRegion(TEXT("Global"));
    return Mods.GatheringYieldMult;
}

float ULootSubsystem::BloodMoonRarityBonus() const
{
    if (!Weather.IsValid())
    {
        return 0.0f;
    }

    const FEnvironmentModifiers Mods = Weather.Get()->GetModifiersForRegion(TEXT("Global"));
    return Mods.SpawnRateMult >= 4.9f ? 0.10f : 0.0f;
}

bool ULootSubsystem::GrantLootToActor(AActor* TargetActor, const FLootResult& Loot) const
{
    if (!TargetActor || Loot.ItemID.IsNone() || Loot.Quantity <= 0)
    {
        return false;
    }

    if (USpatialRingComponent* Ring = TargetActor->FindComponentByClass<USpatialRingComponent>())
    {
        FItemInstance Instance = BuildItemInstance(Loot);
        if (Ring->AddItem(Instance))
        {
            NotifyQuestProgress(TargetActor, Loot);
            return true;
        }
    }

    return false;
}

bool ULootSubsystem::GrantLootToCharacter(const FString& CharacterID, const FLootResult& Loot, const FPartyInfo& PartyInfo)
{
    if (CharacterID.IsEmpty())
    {
        return false;
    }

    AActor* TargetActor = nullptr;
    for (const FPartyMember& Member : PartyInfo.Members)
    {
        if (Member.CharacterID == CharacterID)
        {
            if (Member.Pawn.IsValid())
            {
                TargetActor = Member.Pawn.Get();
            }
            break;
        }
    }

    if (!TargetActor)
    {
        if (UWorld* World = GetWorld())
        {
            for (TActorIterator<AActor> It(World); It; ++It)
            {
                if (const UActionSaveRouterComponent* Router = It->FindComponentByClass<UActionSaveRouterComponent>())
                {
                    if (Router->CharacterID == CharacterID)
                    {
                        TargetActor = *It;
                        break;
                    }
                }
            }
        }
    }

    return GrantLootToActor(TargetActor, Loot);
}

void ULootSubsystem::NotifyQuestProgress(AActor* TargetActor, const FLootResult& Loot) const
{
    if (!TargetActor)
    {
        return;
    }

    if (UQuestComponent* Quest = TargetActor->FindComponentByClass<UQuestComponent>())
    {
        Quest->OnCollect(Loot.ItemID, Loot.Quantity);
    }
}

FString ULootSubsystem::ResolveCharacterId(const AActor* Actor) const
{
    if (!Actor)
    {
        return FString();
    }

    if (const UActionSaveRouterComponent* Router = Actor->FindComponentByClass<UActionSaveRouterComponent>())
    {
        if (!Router->CharacterID.IsEmpty())
        {
            return Router->CharacterID;
        }
    }

    if (const FProperty* IdentityProp = Actor->GetClass()->FindPropertyByName(TEXT("CharacterIdentity")))
    {
        if (const FStructProperty* StructProp = CastField<const FStructProperty>(IdentityProp))
        {
            const void* StructPtr = IdentityProp->ContainerPtrToValuePtr<void>(Actor);
            if (StructPtr)
            {
                if (const FProperty* CharIdProp = StructProp->Struct->FindPropertyByName(TEXT("CharacterID")))
                {
                    if (const FStrProperty* StrProp = CastField<const FStrProperty>(CharIdProp))
                    {
                        return StrProp->GetPropertyValue_InContainer(StructPtr);
                    }
                }
            }
        }
    }

    return FString();
}

FString ULootSubsystem::ResolveAccountId(const AActor* Actor) const
{
    if (!Actor)
    {
        return FString();
    }

    if (const UActionSaveRouterComponent* Router = Actor->FindComponentByClass<UActionSaveRouterComponent>())
    {
        return Router->AccountID;
    }

    return FString();
}

FName ULootSubsystem::ResolveDropTableId(const AActor* Actor) const
{
    if (!Actor)
    {
        return NAME_None;
    }

    if (const FProperty* DropProp = Actor->GetClass()->FindPropertyByName(TEXT("DropTableID")))
    {
        if (const FNameProperty* NameProp = CastField<const FNameProperty>(DropProp))
        {
            const FName Value = NameProp->GetPropertyValue_InContainer(Actor);
            if (!Value.IsNone())
            {
                return Value;
            }
        }
    }

    for (const FName& Tag : Actor->Tags)
    {
        const FString TagStr = Tag.ToString();
        FString Key;
        FString Value;
        if (TagStr.Split(TEXT("="), &Key, &Value) || TagStr.Split(TEXT(":"), &Key, &Value))
        {
            if (Key.Equals(TEXT("DropTable"), ESearchCase::IgnoreCase))
            {
                return FName(*Value);
            }
        }
    }

    return NAME_None;
}

void ULootSubsystem::ResolveRoundRobinTrash(FLootSession& Session, FPartyInfo& PartyInfo)
{
    if (PartyInfo.Members.IsEmpty())
    {
        return;
    }

    for (const FLootResult& Loot : Session.Trash)
    {
        const int32 Index = PartyInfo.RoundRobinIndex % PartyInfo.Members.Num();
        const FString& Recipient = PartyInfo.Members[Index].CharacterID;
        GrantLootToCharacter(Recipient, Loot, PartyInfo);
        ++PartyInfo.RoundRobinIndex;
    }

    Session.Trash.Empty();
}

void ULootSubsystem::AutoListExpiredSessions()
{
    if (!Auction.IsValid())
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const double Now = World->GetTimeSeconds();
    TArray<FString> ExpiredSessions;

    for (const TPair<FString, FLootSession>& Pair : Sessions)
    {
        if (Pair.Value.ExpireAtWorldSeconds > 0.0 && Now >= Pair.Value.ExpireAtWorldSeconds)
        {
            ExpiredSessions.Add(Pair.Key);
        }
    }

    for (const FString& SessionID : ExpiredSessions)
    {
        FLootSession SessionCopy;
        if (FLootSession* Session = Sessions.Find(SessionID))
        {
            SessionCopy = *Session;
            Sessions.Remove(SessionID);
        }
        else
        {
            continue;
        }

        PendingRolls.Remove(SessionID);

        if (!Party.IsValid())
        {
            continue;
        }

        FPartyInfo PartyInfo;
        if (!Party.Get()->GetPartyByID(SessionCopy.PartyID, PartyInfo))
        {
            continue;
        }

        TArray<FString> Beneficiaries;
        for (const FPartyMember& Member : PartyInfo.Members)
        {
            if (!Member.CharacterID.IsEmpty())
            {
                Beneficiaries.Add(Member.CharacterID);
            }
        }

        for (const FLootResult& Loot : SessionCopy.NonTrash)
        {
            FAuctionListing Listing;
            Listing.BeneficiaryCharacterIDs = Beneficiaries;
            Listing.ItemID = Loot.ItemID;
            Listing.Quantity = Loot.Quantity;
            Listing.Rarity = Loot.Rarity;
            Auction.Get()->CreateListing(Listing);
        }
    }
}

void ULootSubsystem::EvaluateOpenRolls(const FString& SessionID, FLootSession& Session)
{
    if (TMap<int32, TMap<FString, int32>>* Rolls = PendingRolls.Find(SessionID))
    {
        TArray<int32> InvalidIndices;
        for (const TPair<int32, TMap<FString, int32>>& Pair : *Rolls)
        {
            if (!Session.NonTrash.IsValidIndex(Pair.Key))
            {
                InvalidIndices.Add(Pair.Key);
            }
        }

        for (int32 Index : InvalidIndices)
        {
            Rolls->Remove(Index);
        }

        if (Rolls->IsEmpty())
        {
            PendingRolls.Remove(SessionID);
        }
    }
}

FLootSession& ULootSubsystem::CreateSession(const FString& PartyID, const FString& MasterID, const FString& KillerCharID)
{
    FLootSession Session;
    Session.SessionID = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphensInBraces);
    Session.PartyID = PartyID;
    Session.MasterAccountID = MasterID;
    Session.KillerCharacterID = KillerCharID;

    if (UWorld* World = GetWorld())
    {
        Session.ExpireAtWorldSeconds = World->GetTimeSeconds() + 1800.0;
    }

    return Sessions.Add(Session.SessionID, Session);
}

void ULootSubsystem::RemoveSessionIfEmpty(const FString& SessionID, FLootSession& Session)
{
    if (Session.NonTrash.IsEmpty() && Session.Trash.IsEmpty())
    {
        Sessions.Remove(SessionID);
        PendingRolls.Remove(SessionID);
    }
}

void ULootSubsystem::FinalizeRollWinner(const FString& SessionID, int32 LootIndex, FLootSession& Session, TMap<FString, int32>& Rolls, const FPartyInfo& PartyInfo)
{
    FString WinnerCharacterID;
    int32 BestRoll = -1;

    for (const TPair<FString, int32>& Pair : Rolls)
    {
        if (Pair.Value > BestRoll)
        {
            BestRoll = Pair.Value;
            WinnerCharacterID = Pair.Key;
        }
    }

    if (WinnerCharacterID.IsEmpty())
    {
        return;
    }

    if (!GrantLootToCharacter(WinnerCharacterID, Session.NonTrash[LootIndex], PartyInfo))
    {
        return;
    }

    Session.NonTrash.RemoveAt(LootIndex);
    Rolls.Empty();

    if (TMap<int32, TMap<FString, int32>>* SessionRolls = PendingRolls.Find(SessionID))
    {
        SessionRolls->Remove(LootIndex);
        if (SessionRolls->IsEmpty())
        {
            PendingRolls.Remove(SessionID);
        }
    }

    RemoveSessionIfEmpty(SessionID, Session);
}

EItemRarity ULootSubsystem::ToItemRarity(ELootRarity Rarity)
{
    switch (Rarity)
    {
        case ELootRarity::Trash:      return EItemRarity::Poor;
        case ELootRarity::Common:     return EItemRarity::Normal;
        case ELootRarity::Uncommon:   return EItemRarity::Greater;
        case ELootRarity::Rare:       return EItemRarity::Rare;
        case ELootRarity::Epic:       return EItemRarity::Legendary;
        case ELootRarity::Legendary:  return EItemRarity::Immortal;
        default:                      return EItemRarity::Normal;
    }
}

FItemInstance ULootSubsystem::BuildItemInstance(const FLootResult& Result) const
{
    FItemDefinition Definition;
    Definition.ItemID = Result.ItemID;
    Definition.MaxStack = FMath::Max(Result.Quantity, 1);
    Definition.Rarity = ToItemRarity(Result.Rarity);

    FItemInstance Instance;
    Instance.Definition = Definition;
    Instance.Quantity = Result.Quantity;
    return Instance;
}
