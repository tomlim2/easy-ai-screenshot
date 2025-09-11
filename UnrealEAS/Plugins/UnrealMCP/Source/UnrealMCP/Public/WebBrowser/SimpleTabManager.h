#pragma once

#include "CoreMinimal.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"

/**
 * Simple Tab Manager - minimal implementation
 */
class UNREALMCP_API FSimpleTabManager
{
public:
    static void Initialize();
    static void Shutdown();
    static void InvokeWebBrowserTab();

private:
    static const FName WebBrowserTabId;
    static TSharedRef<SDockTab> SpawnWebBrowserTab(const FSpawnTabArgs& Args);
    static void RegisterTabSpawner();
    static void UnregisterTabSpawner();
    
    static bool bIsInitialized;
};