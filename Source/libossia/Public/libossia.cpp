#include "libossia.h"

#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include <ossia/context.hpp>

#define LOCTEXT_NAMESPACE "FlibossiaModule"

ossia::context* g_ossia_ctx{};
void FlibossiaModule::StartupModule()
{
	g_ossia_context = new ossia::context;
	/*
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("libossia")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/libossiaLibrary/Win64/ExampleLibrary.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/libossiaLibrary/Mac/Release/libExampleLibrary.dylib"));
#else
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/libossiaLibrary/Mac/Release/libExampleLibrary.dylib")); 
#endif // PLATFORM_WINDOWS

	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (ExampleLibraryHandle)
	{
		// Call the test function in the third party library that opens a message box
		//ExampleLibraryFunction();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
	*/
}

void FlibossiaModule::ShutdownModule()
{
	delete g_ossia_context;
	/*
	
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	ExampleLibraryHandle = nullptr;
	*/
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FlibossiaModule, libossia)
