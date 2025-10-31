using System.IO;
using UnrealBuildTool;

public class WuxiaMMO : ModuleRules
{
    public WuxiaMMO(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine", "InputCore",
                "EnhancedInput",
                "GameplayAbilities", "GameplayTags", "GameplayTasks",
                "HTTP", "Json", "JsonUtilities",
                "UMG", "Slate", "SlateCore",
                "Niagara",
                "Perception",
                "NavigationSystem",
                "AIModule"
            });

        PublicIncludePaths.AddRange(
            new string[]
            {
                Path.Combine(ModuleDirectory, "Components"),
                Path.Combine(ModuleDirectory, "Data"),
                Path.Combine(ModuleDirectory, "Systems")
            });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
