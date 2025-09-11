#include "WebBrowser/SimpleTabManager.h"
#include "WebBrowser/SimpleWebBrowserTab.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"

const FName FSimpleTabManager::WebBrowserTabId = FName("SimpleWebBrowser");
bool FSimpleTabManager::bIsInitialized = false;

void FSimpleTabManager::Initialize()
{
    if (!bIsInitialized)
    {
        RegisterTabSpawner();
        bIsInitialized = true;
        UE_LOG(LogTemp, Log, TEXT("FSimpleTabManager: Initialized"));
    }
}

void FSimpleTabManager::Shutdown()
{
    if (bIsInitialized)
    {
        UnregisterTabSpawner();
        bIsInitialized = false;
        UE_LOG(LogTemp, Log, TEXT("FSimpleTabManager: Shut down"));
    }
}

void FSimpleTabManager::RegisterTabSpawner()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();

    if (LevelEditorTabManager.IsValid())
    {
        LevelEditorTabManager->RegisterTabSpawner(WebBrowserTabId, 
            FOnSpawnTab::CreateStatic(&FSimpleTabManager::SpawnWebBrowserTab))
            .SetDisplayName(NSLOCTEXT("UnrealMCP", "SimpleWebBrowserTabTitle", "Simple Web Browser"))
            .SetTooltipText(NSLOCTEXT("UnrealMCP", "SimpleWebBrowserTabTooltip", "Open Simple Web Browser"))
            .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"))
            .SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsDebugCategory());

        UE_LOG(LogTemp, Log, TEXT("FSimpleTabManager: Tab spawner registered"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FSimpleTabManager: Failed to get Level Editor Tab Manager"));
    }
}

void FSimpleTabManager::UnregisterTabSpawner()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();

    if (LevelEditorTabManager.IsValid())
    {
        LevelEditorTabManager->UnregisterTabSpawner(WebBrowserTabId);
        UE_LOG(LogTemp, Log, TEXT("FSimpleTabManager: Tab spawner unregistered"));
    }
}

TSharedRef<SDockTab> FSimpleTabManager::SpawnWebBrowserTab(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        .Label(NSLOCTEXT("UnrealMCP", "SimpleWebBrowserTabTitle", "Simple Web Browser"))
        [
            SNew(SSimpleWebBrowserTab)
            .InitialURL(TEXT("http://localhost:3000"))
        ];
}

void FSimpleTabManager::InvokeWebBrowserTab()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();

    if (LevelEditorTabManager.IsValid())
    {
        LevelEditorTabManager->TryInvokeTab(WebBrowserTabId);
        UE_LOG(LogTemp, Log, TEXT("FSimpleTabManager: Web browser tab invoked"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FSimpleTabManager: Failed to invoke tab - no tab manager"));
    }
}