using UnrealBuildTool;

public class DataTableFunctionLibrary : ModuleRules {
    public DataTableFunctionLibrary(ReadOnlyTargetRules Target) : base(Target) {
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
        });
    }
}
