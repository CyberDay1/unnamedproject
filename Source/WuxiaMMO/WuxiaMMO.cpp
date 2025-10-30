#include "WuxiaMMO.h"
#include "Modules/ModuleManager.h"

#include "Components/CultivationComponent.h"
#include "Systems/AlchemySubsystem.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FWuxiaMMOModule, WuxiaMMO, "WuxiaMMO");

void FWuxiaMMOModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("WuxiaMMO module initialized: Cultivation and Alchemy systems ready."));
}

void FWuxiaMMOModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("WuxiaMMO module shutdown: Cultivation and Alchemy systems cleaned up."));
}
