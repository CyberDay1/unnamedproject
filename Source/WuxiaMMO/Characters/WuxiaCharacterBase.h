#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WuxiaCharacterBase.generated.h"

UCLASS()
class AWuxiaCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AWuxiaCharacterBase();

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
};
