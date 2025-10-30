#include "Components/CurrencyComponent.h"
#include "Components/CultivationComponent.h"
#include "Components/ReputationComponent.h"
#include "GameFramework/Actor.h"
#include "Systems/ActionSaveRouterComponent.h"

static void SaveTriggerFromOwner(UActorComponent* Component, const TCHAR* Reason)
{
    if (!Component)
    {
        return;
    }

    if (AActor* Owner = Component->GetOwner())
    {
        if (UActionSaveRouterComponent* Router = Owner->FindComponentByClass<UActionSaveRouterComponent>())
        {
            Router->TriggerSave(Reason);
        }
    }
}

UCurrencyComponent::UCurrencyComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCurrencyComponent::BeginPlay()
{
    Super::BeginPlay();

    auto SetUnit = [&](ECurrencyType T, int32 Order, int32 Ratio, float Qi, ECultivationStage MinStage)
    {
        FCurrencyUnit U;
        U.OrderIndex = Order;
        U.RatioToNext = Ratio;
        U.BaseQiGrant = Qi;
        U.MinStageForFullEffect = MinStage;
        Units.Add(T, U);
    };

    SetUnit(ECurrencyType::SilverTael, 0, 10, 0.f, ECultivationStage::MortalBody);
    SetUnit(ECurrencyType::GoldTael, 1, 10, 0.f, ECultivationStage::MortalBody);

    SetUnit(ECurrencyType::SmallSpiritStone_Poor, 2, 10, 5.f, ECultivationStage::MortalBody);
    SetUnit(ECurrencyType::SmallSpiritStone_Average, 3, 10, 8.f, ECultivationStage::QiRefining);
    SetUnit(ECurrencyType::SmallSpiritStone_Rich, 4, 10, 12.f, ECultivationStage::FoundationEstablishment);

    SetUnit(ECurrencyType::LargeSpiritStone_Poor, 5, 10, 20.f, ECultivationStage::CoreFormation);
    SetUnit(ECurrencyType::LargeSpiritStone_Average, 6, 10, 28.f, ECultivationStage::NascentSoul);
    SetUnit(ECurrencyType::LargeSpiritStone_Rich, 7, 10, 36.f, ECultivationStage::SoulTransformation);

    SetUnit(ECurrencyType::ImmortalStone_Poor, 8, 100, 80.f, ECultivationStage::ImmortalAscension);
    SetUnit(ECurrencyType::ImmortalStone_Average, 9, 100, 120.f, ECultivationStage::TrueImmortal);
    SetUnit(ECurrencyType::ImmortalStone_Rich, 10, 100, 180.f, ECultivationStage::HeavenlyImmortal);
    SetUnit(ECurrencyType::ImmortalStone_Overflowing, 11, 100, 260.f, ECultivationStage::GoldenImmortal);

    FCurrencyUnit Contrib;
    Contrib.OrderIndex = -1;
    Contrib.RatioToNext = 1;
    Contrib.BaseQiGrant = 0.f;
    Contrib.MinStageForFullEffect = ECultivationStage::MortalBody;
    Units.Add(ECurrencyType::SectContribution, Contrib);
}

void UCurrencyComponent::Add(ECurrencyType Type, int64 Amount)
{
    int64& Bal = Balances.FindOrAdd(Type);
    Bal = FMath::Max<int64>(0, Bal + Amount);
    SaveTriggerFromOwner(this, TEXT("Currency:Add"));
}

bool UCurrencyComponent::Remove(ECurrencyType Type, int64 Amount)
{
    int64* Bal = Balances.Find(Type);
    if (!Bal || *Bal < Amount)
    {
        return false;
    }

    *Bal -= Amount;
    SaveTriggerFromOwner(this, TEXT("Currency:Remove"));
    return true;
}

int32 UCurrencyComponent::GetOrder(ECurrencyType Type) const
{
    if (const FCurrencyUnit* U = Units.Find(Type))
    {
        return U->OrderIndex;
    }
    return -1;
}

bool UCurrencyComponent::GetNextHigher(ECurrencyType Type, ECurrencyType& OutNext) const
{
    const int32 Curr = GetOrder(Type);
    if (Curr < 0)
    {
        return false;
    }

    for (int32 i = static_cast<int32>(ECurrencyType::SilverTael); i <= static_cast<int32>(ECurrencyType::ImmortalStone_Overflowing); ++i)
    {
        const ECurrencyType Candidate = static_cast<ECurrencyType>(i);
        if (GetOrder(Candidate) == Curr + 1)
        {
            OutNext = Candidate;
            return true;
        }
    }

    return false;
}

bool UCurrencyComponent::GetNextLower(ECurrencyType Type, ECurrencyType& OutPrev) const
{
    const int32 Curr = GetOrder(Type);
    if (Curr <= 0)
    {
        return false;
    }

    for (int32 i = static_cast<int32>(ECurrencyType::SilverTael); i <= static_cast<int32>(ECurrencyType::ImmortalStone_Overflowing); ++i)
    {
        const ECurrencyType Candidate = static_cast<ECurrencyType>(i);
        if (GetOrder(Candidate) == Curr - 1)
        {
            OutPrev = Candidate;
            return true;
        }
    }

    return false;
}

int64 UCurrencyComponent::ConvertUp(ECurrencyType FromType, int64 MaxSteps)
{
    ECurrencyType Next;
    if (!GetNextHigher(FromType, Next))
    {
        return 0;
    }

    const FCurrencyUnit* Unit = Units.Find(FromType);
    if (!Unit)
    {
        return 0;
    }

    int64& BalFrom = Balances.FindOrAdd(FromType);
    int64& BalNext = Balances.FindOrAdd(Next);

    const int32 Ratio = Unit->RatioToNext;
    const int64 Possible = BalFrom / Ratio;
    const int64 ToConvert = FMath::Min<int64>(Possible, MaxSteps);
    if (ToConvert <= 0)
    {
        return 0;
    }

    BalFrom -= ToConvert * Ratio;
    BalNext += ToConvert;
    return ToConvert;
}

int64 UCurrencyComponent::ConvertDown(ECurrencyType FromType, int64 UnitsToBreak)
{
    ECurrencyType Prev;
    if (!GetNextLower(FromType, Prev))
    {
        return 0;
    }

    const FCurrencyUnit* PrevUnit = Units.Find(Prev);
    if (!PrevUnit)
    {
        return 0;
    }

    int64& BalFrom = Balances.FindOrAdd(FromType);
    int64& BalPrev = Balances.FindOrAdd(Prev);

    if (BalFrom < UnitsToBreak)
    {
        return 0;
    }

    BalFrom -= UnitsToBreak;
    const int64 LowerUnits = UnitsToBreak * PrevUnit->RatioToNext;
    BalPrev += LowerUnits;
    return LowerUnits;
}

bool UCurrencyComponent::ConsumeStoneForQi(ECurrencyType StoneType, UCultivationComponent* Cultivation, float& OutQiGained)
{
    OutQiGained = 0.f;
    if (!Cultivation)
    {
        return false;
    }

    if (StoneType == ECurrencyType::SilverTael || StoneType == ECurrencyType::GoldTael || StoneType == ECurrencyType::SectContribution)
    {
        return false;
    }

    const FCurrencyUnit* Unit = Units.Find(StoneType);
    if (!Unit)
    {
        return false;
    }

    int64* Balance = Balances.Find(StoneType);
    if (!Balance || *Balance <= 0)
    {
        return false;
    }

    const uint8 PlayerStage = static_cast<uint8>(Cultivation->Progress.Stage);
    const uint8 NeededStage = static_cast<uint8>(Unit->MinStageForFullEffect);

    if (PlayerStage < NeededStage)
    {
        return false;
    }

    const bool bBelowTier = PlayerStage > NeededStage;
    const float Effectiveness = bBelowTier ? 0.25f : 1.0f;

    *Balance -= 1;
    OutQiGained = Unit->BaseQiGrant * Effectiveness;
    SaveTriggerFromOwner(this, TEXT("Currency:ConsumeStone"));
    return true;
}

float UCurrencyComponent::PriceMultiplierFromReputation(const UReputationComponent* Rep, FName SectID, float Base)
{
    if (!Rep)
    {
        return Base;
    }

    float ReputationValue = 0.f;
    if (SectID != NAME_None)
    {
        ReputationValue = Rep->GetSectReputation(SectID);
    }

    const float Normalized = FMath::Clamp(ReputationValue / 5000.f, -1.f, 1.f);
    const float Multiplier = 1.f - 0.2f * Normalized;
    return Base * Multiplier;
}
