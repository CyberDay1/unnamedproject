#include "AI/BTService_BloodMoonEnrage.h"
#include "AI/BaseAIController.h"
#include "Environment/WeatherSubsystem.h"

UBTService_BloodMoonEnrage::UBTService_BloodMoonEnrage()
{
    Interval = 0.8f;
    RandomDeviation = 0.2f;
}

void UBTService_BloodMoonEnrage::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float)
{
    ABaseAIController* C = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!C) return;

    if (UGameInstance* GI = C->GetGameInstance())
    {
        if (UWeatherSubsystem* WS = GI->GetSubsystem<UWeatherSubsystem>())
        {
            const FEnvironmentModifiers GlobalMods = WS->GetModifiersForRegion(TEXT("Global"));
            const bool bBlood = GlobalMods.SpawnRateMult >= 4.9f; // Blood Moon sets 5.0x
            C->ApplyBloodMoonAggro(bBlood);
            if (bBlood) C->SetBlackboardState(EAIState::Enraged);
        }
    }
}
