#include "UnrealMCPModule.h"
#include "UnrealMCPBridge.h"
#include "Modules/ModuleManager.h"
#include "EditorSubsystem.h"
#include "Editor.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FUnrealMCPModule"

void FUnrealMCPModule::StartupModule()
{
	UE_LOG(LogTemp, Display, TEXT("Unreal MCP Module has started"));

	// Register menu extensions
	if (!IsRunningCommandlet())
	{
		RegisterMenuExtensions();
	}
}

void FUnrealMCPModule::ShutdownModule()
{
	// Unregister menu extensions
	UnregisterMenuExtensions();

	UE_LOG(LogTemp, Display, TEXT("Unreal MCP Module has shut down"));
}

void FUnrealMCPModule::RegisterMenuExtensions()
{
	// Wait for the tool menus module to be loaded
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUnrealMCPModule::OnToolMenusLoaded));
}

void FUnrealMCPModule::UnregisterMenuExtensions()
{
	if (UToolMenus* ToolMenus = UToolMenus::Get())
	{
		// Remove any registered menus
		ToolMenus->RemoveSection("LevelEditor.LevelEditorToolBar.User", "UnrealMCP");
	}
}

void FUnrealMCPModule::OnToolMenusLoaded()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<FUICommandList> CommandList = LevelEditorModule.GetGlobalLevelEditorActions();

	// Register toolbar button
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	if (ToolbarMenu)
	{
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("UnrealMCP");
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(
			"OpenAIScreenshotTool",
			FUIAction(FExecuteAction::CreateStatic(&FUnrealMCPModule::OnOpenAIScreenshotTool)),
			LOCTEXT("OpenAIScreenshotTool", "AI Screenshot"),
			LOCTEXT("OpenAIScreenshotToolTooltip", "Open the AI Screenshot Tool"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details")
		));
	}
}

void FUnrealMCPModule::OnOpenAIScreenshotTool()
{
	// 시스템 기본 브라우저로 NextJS 앱 열기
	FPlatformProcess::LaunchURL(TEXT("http://localhost:3000"), nullptr, nullptr);
	UE_LOG(LogTemp, Log, TEXT("FUnrealMCPModule: Opened AI Screenshot Tool in external browser"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealMCPModule, UnrealMCP) 