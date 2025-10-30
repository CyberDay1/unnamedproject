#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NPCInteractionComponent.generated.h"

// Attach to NPC actors; stores UUID and exposes simple interact API
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNPCInteractionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC")
    FString NPCUUID;

    UFUNCTION(BlueprintCallable, Category="NPC")
    bool BeginDialogue(AActor* PlayerActor);
};
