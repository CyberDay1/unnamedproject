#include "Systems/InventorySubsystem.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UInventorySubsystem::SaveInventory(const FString& CharacterID, const TArray<FItemInstance>& Items)
{
    FString SaveDir = FPaths::ProjectSavedDir() / TEXT("Inventories");
    FString SavePath = SaveDir / (CharacterID + TEXT("_Inventory.sav"));

    FString Output;
    for (const FItemInstance& Item : Items)
    {
        Output += FString::Printf(TEXT("%s,%d,%d\n"), *Item.Definition.ItemID.ToString(), Item.Quantity, (int32)Item.Definition.Rarity);
    }
    FFileHelper::SaveStringToFile(Output, *SavePath);
}

bool UInventorySubsystem::LoadInventory(const FString& CharacterID, TArray<FItemInstance>& OutItems) const
{
    FString SaveDir = FPaths::ProjectSavedDir() / TEXT("Inventories");
    FString SavePath = SaveDir / (CharacterID + TEXT("_Inventory.sav"));

    FString Input;
    if (!FFileHelper::LoadFileToString(Input, *SavePath))
        return false;

    TArray<FString> Lines;
    Input.ParseIntoArrayLines(Lines);
    for (const FString& Line : Lines)
    {
        TArray<FString> Parts;
        Line.ParseIntoArray(Parts, TEXT(","));
        if (Parts.Num() >= 3)
        {
            FItemDefinition Def;
            Def.ItemID = FName(*Parts[0]);
            Def.Rarity = static_cast<EItemRarity>(FCString::Atoi(*Parts[2]));

            FItemInstance Inst;
            Inst.Definition = Def;
            Inst.Quantity = FCString::Atoi(*Parts[1]);
            OutItems.Add(Inst);
        }
    }
    return true;
}
