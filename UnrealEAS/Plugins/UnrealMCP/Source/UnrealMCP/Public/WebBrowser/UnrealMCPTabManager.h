#pragma once

#include "CoreMinimal.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "WorkspaceMenuStructure.h"

class SUnrealMCPWebBrowserTab;
class FUnrealMCPWebBridge_Simple;

/**
 * Tab Manager for Unreal MCP Web Browser
 * Handles creation and management of dockable web browser tabs
 */
class UNREALMCP_API FUnrealMCPTabManager : public TSharedFromThis<FUnrealMCPTabManager>
{
public:
    /** Initialize the tab manager */
    static void Initialize();

    /** Shutdown the tab manager */
    static void Shutdown();

    /** Get the singleton instance */
    static FUnrealMCPTabManager& Get();

    /** Register the web browser tab spawner */
    void RegisterTabSpawner();

    /** Unregister the web browser tab spawner */
    void UnregisterTabSpawner();

    /** Spawn the web browser tab */
    TSharedRef<SDockTab> SpawnWebBrowserTab(const FSpawnTabArgs& Args);

    /** Static spawn function for tab registration */
    static TSharedRef<SDockTab> StaticSpawnWebBrowserTab(const FSpawnTabArgs& Args);

    /** Invoke the web browser tab (open or focus existing) */
    void InvokeWebBrowserTab();

    /** Check if the web browser tab is currently open */
    bool IsWebBrowserTabOpen() const;

    /** Get the current web browser tab */
    TSharedPtr<SDockTab> GetWebBrowserTab() const;

    /** Get the web browser widget from the current tab */
    TSharedPtr<SUnrealMCPWebBrowserTab> GetWebBrowserWidget() const;

    /** Get the web bridge object */
    TSharedPtr<FUnrealMCPWebBridge_Simple> GetWebBridge() const;

public:
    ~FUnrealMCPTabManager() = default;

private:
    FUnrealMCPTabManager() = default;

    /** Singleton instance */
    static TUniquePtr<FUnrealMCPTabManager> Instance;

    /** Tab ID for the web browser */
    static const FName WebBrowserTabId;

    /** Current web browser tab */
    TWeakPtr<SDockTab> CurrentWebBrowserTab;

    /** Current web browser widget */
    TWeakPtr<SUnrealMCPWebBrowserTab> CurrentWebBrowserWidget;

    /** Web bridge object */
    TSharedPtr<FUnrealMCPWebBridge_Simple> WebBridge;

    /** Handle tab closed event */
    void OnWebBrowserTabClosed(TSharedRef<SDockTab> ClosedTab);
};