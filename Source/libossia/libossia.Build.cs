// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class libossia : ModuleRules
{
	public libossia(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.NoSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		ShadowVariableWarningLevel = WarningLevel.Off;
		bUseRTTI = true;
		PublicDefinitions.AddRange(
			new string[] { 
				"__clang_major___WORKAROUND_GUARD=0",
				"__NVCC___WORKAROUND_GUARD=1",
				"__cpp_coroutines=0",
				"EGGS_CXX98_HAS_RTTI=0",
				"RAPIDJSON_HAS_STDSTRING=1",
				"ASIO_DISABLE_CONCEPTS=1",
				"NOMINMAX",
				"_CRT_SECURE_NO_WARNINGS",
				"WIN32_LEAN_AND_MEAN",
				"ASIO_STANDALONE=1",
				"FMT_HEADER_ONLY=1"
		});

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
