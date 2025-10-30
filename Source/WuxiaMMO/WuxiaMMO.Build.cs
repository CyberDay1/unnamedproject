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
                "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks"
            });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
