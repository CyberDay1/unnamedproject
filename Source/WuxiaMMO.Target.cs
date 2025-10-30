using UnrealBuildTool;
using System.Collections.Generic;

public class WuxiaMMOTarget : TargetRules
{
    public WuxiaMMOTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.AddRange(new string[] { "WuxiaMMO" });
    }
}
