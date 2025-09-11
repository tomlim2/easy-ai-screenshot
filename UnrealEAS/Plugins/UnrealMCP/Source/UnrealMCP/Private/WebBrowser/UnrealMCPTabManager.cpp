#include "WebBrowser/UnrealMCPTabManager.h"
#include "WebBrowser/UnrealMCPWebBrowserTab.h"
#include "WebBrowser/UnrealMCPWebBridge_Simple.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Framework/Application/SlateApplication.h"
#include "LevelEditor.h"

const FName FUnrealMCPTabManager::WebBrowserTabId = FName("UnrealMCPWebBrowser");
TUniquePtr<FUnrealMCPTabManager> FUnrealMCPTabManager::Instance = nullptr;

void FUnrealMCPTabManager::Initialize()
{
    if (!Instance.IsValid())
    {
        Instance = TUniquePtr<FUnrealMCPTabManager>(new FUnrealMCPTabManager());
        Instance->RegisterTabSpawner();
        UE_LOG(LogTemp, Log, TEXT("FUnrealMCPTabManager: Initialized"));
    }
}

void FUnrealMCPTabManager::Shutdown()
{
    if (Instance.IsValid())
    {
        Instance->UnregisterTabSpawner();
        Instance.Reset();
        UE_LOG(LogTemp, Log, TEXT("FUnrealMCPTabManager: Shut down"));
    }
}

FUnrealMCPTabManager& FUnrealMCPTabManager::Get()
{
    check(Instance.IsValid());
    return *Instance;
}

void FUnrealMCPTabManager::RegisterTabSpawner()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();

    if (LevelEditorTabManager.IsValid())
    {
        LevelEditorTabManager->RegisterTabSpawner(WebBrowserTabId, 
            FOnSpawnTab::CreateStatic(&FUnrealMCPTabManager::StaticSpawnWebBrowserTab))
            .SetDisplayName(NSLOCTEXT("UnrealMCP", "WebBrowserTabTitle", "AI Screenshot Tool"))
            .SetTooltipText(NSLOCTEXT("UnrealMCP", "WebBrowserTabTooltip", "Open the AI Screenshot Tool web interface"))
            .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"))
            .SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsDebugCategory());

        UE_LOG(LogTemp, Log, TEXT("FUnrealMCPTabManager: Tab spawner registered"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FUnrealMCPTabManager: Failed to get Level Editor Tab Manager"));
    }
}

void FUnrealMCPTabManager::UnregisterTabSpawner()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();

    if (LevelEditorTabManager.IsValid())
    {
        LevelEditorTabManager->UnregisterTabSpawner(WebBrowserTabId);
        UE_LOG(LogTemp, Log, TEXT("FUnrealMCPTabManager: Tab spawner unregistered"));
    }
}

TSharedRef<SDockTab> FUnrealMCPTabManager::StaticSpawnWebBrowserTab(const FSpawnTabArgs& Args)
{
    if (Instance.IsValid())
    {
        return Instance->SpawnWebBrowserTab(Args);
    }
    
    // Fallback: create a simple tab
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        .Label(NSLOCTEXT("UnrealMCP", "WebBrowserTabTitle", "AI Screenshot Tool"))
        [
            SNew(STextBlock)
            .Text(NSLOCTEXT("UnrealMCP", "InitializationError", "TabManager not initialized"))
        ];
}

TSharedRef<SDockTab> FUnrealMCPTabManager::SpawnWebBrowserTab(const FSpawnTabArgs& Args)
{
    // Create the web bridge if it doesn't exist
    if (!WebBridge.IsValid())
    {
        WebBridge = MakeShared<FUnrealMCPWebBridge_Simple>();
        UE_LOG(LogTemp, Log, TEXT("FUnrealMCPTabManager: Created web bridge object"));
    }

    // Create the web browser widget
    TSharedRef<SUnrealMCPWebBrowserTab> WebBrowserWidget = SNew(SUnrealMCPWebBrowserTab)
        .InitialURL(TEXT("http://localhost:3000"));

    // Create the dock tab
    TSharedRef<SDockTab> DockTab = SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        .Label(NSLOCTEXT("UnrealMCP", "WebBrowserTabTitle", "AI Screenshot Tool"))
        [
            WebBrowserWidget
        ];

    // Store references
    CurrentWebBrowserTab = DockTab;
    CurrentWebBrowserWidget = WebBrowserWidget;

    // Bind tab closed event
    DockTab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateSP(this, &FUnrealMCPTabManager::OnWebBrowserTabClosed));

    UE_LOG(LogTemp, Log, TEXT("FUnrealMCPTabManager: Web browser tab spawned"));

    return DockTab;
}

void FUnrealMCPTabManager::InvokeWebBrowserTab()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();

    if (LevelEditorTabManager.IsValid())
    {
        LevelEditorTabManager->TryInvokeTab(WebBrowserTabId);
        UE_LOG(LogTemp, Log, TEXT("FUnrealMCPTabManager: Web browser tab invoked"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FUnrealMCPTabManager: Failed to invoke tab - no tab manager"));
    }
}

bool FUnrealMCPTabManager::IsWebBrowserTabOpen() const
{
    return CurrentWebBrowserTab.IsValid();
}

TSharedPtr<SDockTab> FUnrealMCPTabManager::GetWebBrowserTab() const
{
    return CurrentWebBrowserTab.Pin();
}

TSharedPtr<SUnrealMCPWebBrowserTab> FUnrealMCPTabManager::GetWebBrowserWidget() const
{
    return CurrentWebBrowserWidget.Pin();
}

TSharedPtr<FUnrealMCPWebBridge_Simple> FUnrealMCPTabManager::GetWebBridge() const
{
    return WebBridge;
}

void FUnrealMCPTabManager::OnWebBrowserTabClosed(TSharedRef<SDockTab> ClosedTab)
{
    UE_LOG(LogTemp, Log, TEXT("FUnrealMCPTabManager: Web browser tab closed"));
    
    // Clear references
    CurrentWebBrowserTab.Reset();
    CurrentWebBrowserWidget.Reset();
}