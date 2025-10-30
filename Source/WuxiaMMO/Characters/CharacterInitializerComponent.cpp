#include "Characters/CharacterInitializerComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/CharacterSaveSubsystem.h"
#include "Data/FClassDefinition.h"
#include "Engine/DataTable.h"

UCharacterInitializerComponent::UCharacterInitializerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    OwnerCharacter = nullptr;
}

void UCharacterInitializerComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UCharacterInitializerComponent::InitializeCharacter(const FCharacterIdentity& Identity, const FCharacterStats& Stats)
{
    ApplyStats(Stats);
    ApplyClassDefinition(Identity.ClassType);
    ApplyElementAffinity(Identity.ElementAffinity);
}

void UCharacterInitializerComponent::ApplyStats(const FCharacterStats& Stats)
{
    if (!OwnerCharacter)
    {
        return;
    }

    OwnerCharacter->CustomTimeDilation = 1.0f;
}

void UCharacterInitializerComponent::ApplyClassDefinition(EClassType ClassType)
{
    static const FString TablePath = TEXT("/Game/DataTables/ClassDefinitions.ClassDefinitions");
    UDataTable* ClassTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *TablePath));
    if (!ClassTable)
    {
        return;
    }

    const FString RowName = UEnum::GetValueAsString(ClassType).Replace(TEXT("EClassType::"), TEXT(""));
    FClassDefinition* Definition = ClassTable->FindRow<FClassDefinition>(*RowName, TEXT("InitCharacter"));
    if (!Definition)
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Loaded class definition: %s"), *Definition->ClassName);
}

void UCharacterInitializerComponent::ApplyElementAffinity(EElementType ElementAffinity)
{
    static const TMap<EElementType, FLinearColor> ElementColors = {
        {EElementType::Wood, FLinearColor(0.1f, 0.8f, 0.1f)},
        {EElementType::Fire, FLinearColor(0.9f, 0.2f, 0.1f)},
        {EElementType::Earth, FLinearColor(0.7f, 0.5f, 0.2f)},
        {EElementType::Metal, FLinearColor(0.9f, 0.9f, 0.9f)},
        {EElementType::Water, FLinearColor(0.1f, 0.4f, 0.9f)}
    };

    if (!OwnerCharacter)
    {
        return;
    }

    const FLinearColor* Color = ElementColors.Find(ElementAffinity);
    if (Color)
    {
        if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
        {
            Mesh->SetVectorParameterValueOnMaterials(TEXT("ElementTint"), FVector(Color->R, Color->G, Color->B));
        }
    }
}
