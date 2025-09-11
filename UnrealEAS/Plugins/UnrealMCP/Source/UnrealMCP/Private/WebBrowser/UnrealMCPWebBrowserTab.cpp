#include "WebBrowser/UnrealMCPWebBrowserTab.h"
#include "WebBrowser/UnrealMCPWebBridge_Simple.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/AppStyle.h"
#include "Framework/Application/SlateApplication.h"

void SUnrealMCPWebBrowserTab::Construct(const FArguments& InArgs)
{
    InitialURL = InArgs._InitialURL;
    OnMessageReceived = InArgs._OnMessageReceived;

    // Create the web bridge object - we'll create it when needed
    WebBridge = nullptr;

    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
        .Padding(0)
        [
            SNew(SBox)
            .MinDesiredWidth(800.0f)
            .MinDesiredHeight(600.0f)
            [
                SAssignNew(WebBrowser, SWebBrowser)
                .InitialURL(InitialURL)
                .ShowControls(false)
                .SupportsTransparency(false)
            ]
        ]
    ];

    // Create and set up the web bridge with the browser
    if (WebBrowser.IsValid())
    {
        WebBridge = MakeShared<FUnrealMCPWebBridge_Simple>();
        WebBridge->SetWebBrowser(WebBrowser);
        
        // Note: JavaScript binding will be handled differently in the simple version
        UE_LOG(LogTemp, Log, TEXT("SUnrealMCPWebBrowserTab: Created simple web bridge"));
    }
}

void SUnrealMCPWebBrowserTab::ExecuteJavaScript(const FString& Script)
{
    if (WebBrowser.IsValid())
    {
        WebBrowser->ExecuteJavascript(Script);
        UE_LOG(LogTemp, VeryVerbose, TEXT("SUnrealMCPWebBrowserTab: Executed JavaScript: %s"), *Script);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SUnrealMCPWebBrowserTab: Cannot execute JavaScript - WebBrowser is invalid"));
    }
}

void SUnrealMCPWebBrowserTab::NavigateToURL(const FString& URL)
{
    if (WebBrowser.IsValid())
    {
        WebBrowser->LoadURL(URL);
        UE_LOG(LogTemp, Log, TEXT("SUnrealMCPWebBrowserTab: Navigating to URL: %s"), *URL);
    }
}

bool SUnrealMCPWebBrowserTab::IsLoading() const
{
    if (WebBrowser.IsValid())
    {
        return WebBrowser->IsLoading();
    }
    return false;
}

void SUnrealMCPWebBrowserTab::Refresh()
{
    if (WebBrowser.IsValid())
    {
        WebBrowser->Reload();
        UE_LOG(LogTemp, Log, TEXT("SUnrealMCPWebBrowserTab: Refreshing page"));
    }
}

FString SUnrealMCPWebBrowserTab::GetCurrentURL() const
{
    if (WebBrowser.IsValid())
    {
        return WebBrowser->GetUrl();
    }
    return FString();
}

void SUnrealMCPWebBrowserTab::BindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
    if (WebBrowser.IsValid())
    {
        WebBrowser->BindUObject(Name, Object, bIsPermanent);
        UE_LOG(LogTemp, Log, TEXT("SUnrealMCPWebBrowserTab: Bound UObject '%s' to web browser"), *Name);
    }
}

void SUnrealMCPWebBrowserTab::HandleMessageFromWeb(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("SUnrealMCPWebBrowserTab: Received message from web: %s"), *Message);
    
    // Forward the message to any bound delegates
    if (OnMessageReceived.IsBound())
    {
        OnMessageReceived.Execute(Message);
    }
}

void SUnrealMCPWebBrowserTab::OnLoadCompleted()
{
    UE_LOG(LogTemp, Log, TEXT("SUnrealMCPWebBrowserTab: Page load completed"));
    
    // Initialize the JavaScript bridge once the page is loaded
    FString InitScript = TEXT("window.unrealMCP = window.unrealMCP || {}; ")
                       TEXT("window.unrealMCP.receiveMessage = function(msg) { console.log('Unreal message:', msg); }; ")
                       TEXT("window.unrealMCP.receiveScreenshot = function(path) { console.log('Unreal screenshot:', path); }; ")
                       TEXT("console.log('Unreal MCP Bridge initialized');");
    
    ExecuteJavaScript(InitScript);
}

void SUnrealMCPWebBrowserTab::OnLoadError()
{
    UE_LOG(LogTemp, Error, TEXT("SUnrealMCPWebBrowserTab: Page load error"));
}

void SUnrealMCPWebBrowserTab::OnUrlChanged(const FText& InText)
{
    FString NewURL = InText.ToString();
    UE_LOG(LogTemp, Log, TEXT("SUnrealMCPWebBrowserTab: URL changed to: %s"), *NewURL);
}

bool SUnrealMCPWebBrowserTab::OnConsoleMessage(const FString& Message, const FString& Source, int32 Line)
{
    // Log browser console messages to Unreal's log
    UE_LOG(LogTemp, Log, TEXT("WebBrowser Console [%s:%d]: %s"), *Source, Line, *Message);
    
    // Don't suppress the message
    return false;
}