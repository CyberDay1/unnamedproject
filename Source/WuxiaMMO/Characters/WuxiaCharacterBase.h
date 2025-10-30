#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/FCharacterIdentity.h"
#include "Data/FCharacterStats.h"
#include "WuxiaCharacterBase.generated.h"

class UCharacterInitializerComponent;
class UDialogueComponent;
class UReputationComponent;
class UQuestComponent;

UCLASS()
class AWuxiaCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AWuxiaCharacterBase();

    UFUNCTION(BlueprintCallable, Category="Character")
    void InitializeFromData(const FCharacterIdentity& Identity, const FCharacterStats& Stats);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    class UCharacterInitializerComponent* Initializer;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UDialogueComponent* Dialogue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UReputationComponent* Reputation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UQuestComponent* Quest;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float Health = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float Qi = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float Stamina = 100.0f;

    void MoveForward(float Value);
    void MoveRight(float Value);

private:
    UPROPERTY()
    FCharacterIdentity CharacterIdentity;

    UPROPERTY()
    FCharacterStats CharacterStats;
};
