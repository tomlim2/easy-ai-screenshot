#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUnrealMCPModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FUnrealMCPModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FUnrealMCPModule>("UnrealMCP");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("UnrealMCP");
	}

private:
	/** Register menu extensions */
	void RegisterMenuExtensions();

	/** Unregister menu extensions */
	void UnregisterMenuExtensions();

	/** Called when tool menus are loaded */
	void OnToolMenusLoaded();

	/** Static callback for opening AI Screenshot Tool */
	static void OnOpenAIScreenshotTool();
}; 