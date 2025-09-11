#pragma once

#include "CoreMinimal.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SCompoundWidget.h"
#include "Delegates/Delegate.h"

class SWebBrowser;
class FUnrealMCPWebBridge_Simple;

DECLARE_DELEGATE_OneParam(FOnWebBrowserMessage, const FString&);

/**
 * Web Browser Tab Widget for Unreal MCP Plugin
 * Provides a dockable web browser tab that can communicate with NextJS application
 */
class UNREALMCP_API SUnrealMCPWebBrowserTab : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SUnrealMCPWebBrowserTab)
        : _InitialURL(TEXT("http://localhost:3000"))
    {}
        SLATE_ARGUMENT(FString, InitialURL)
        SLATE_EVENT(FOnWebBrowserMessage, OnMessageReceived)
    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs);

    /** Execute JavaScript in the web browser */
    void ExecuteJavaScript(const FString& Script);

    /** Navigate to a specific URL */
    void NavigateToURL(const FString& URL);

    /** Check if the browser is currently loading */
    bool IsLoading() const;

    /** Refresh the current page */
    void Refresh();

    /** Get the current URL */
    FString GetCurrentURL() const;

    /** Bind a UObject to the web browser for JavaScript communication */
    void BindUObject(const FString& Name, UObject* Object, bool bIsPermanent = true);

    /** Handle messages from JavaScript */
    void HandleMessageFromWeb(const FString& Message);

private:
    /** The web browser widget */
    TSharedPtr<SWebBrowser> WebBrowser;

    /** Initial URL to load */
    FString InitialURL;

    /** Delegate for handling messages from web */
    FOnWebBrowserMessage OnMessageReceived;

    /** Bridge object for JavaScript communication */
    TSharedPtr<FUnrealMCPWebBridge_Simple> WebBridge;

    /** Handle page load completed */
    void OnLoadCompleted();

    /** Handle page load error */
    void OnLoadError();

    /** Handle URL changes */
    void OnUrlChanged(const FText& InText);

    /** Handle JavaScript console messages */
    bool OnConsoleMessage(const FString& Message, const FString& Source, int32 Line);
};