#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SkillTesterPawn.generated.h"

class USkillRuntimeComponent;
class USkillMasteryComponent;

UCLASS()
class ASkillTesterPawn : public APawn
{
    GENERATED_BODY()

public:
    ASkillTesterPawn();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skills")
    TObjectPtr<USkillRuntimeComponent> SkillComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skills")
    TObjectPtr<USkillMasteryComponent> MasteryComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skills")
    TArray<FName> AutoLearnSkillIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skills")
    TArray<FName> HotbarSkillIDs;

private:
    void RefreshHotbarFromSkills();
    void UseSlot(int32 Index);
    void UseSlot1();
    void UseSlot2();
    void UseSlot3();
    void UseSlot4();
    void UseSlot5();
    void UseSlot6();
};

