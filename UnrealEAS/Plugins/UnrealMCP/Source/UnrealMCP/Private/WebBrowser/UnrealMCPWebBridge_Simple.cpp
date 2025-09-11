#include "WebBrowser/UnrealMCPWebBridge_Simple.h"
#include "SWebBrowser.h"

#ifdef LogLevel
#undef LogLevel
#endif

FUnrealMCPWebBridge_Simple::FUnrealMCPWebBridge_Simple()
{
    UE_LOG(LogTemp, Log, TEXT("FUnrealMCPWebBridge_Simple: Created"));
}

FUnrealMCPWebBridge_Simple::~FUnrealMCPWebBridge_Simple()
{
    UE_LOG(LogTemp, Log, TEXT("FUnrealMCPWebBridge_Simple: Destroyed"));
}

void FUnrealMCPWebBridge_Simple::SetWebBrowser(TSharedPtr<SWebBrowser> InWebBrowser)
{
    WebBrowserPtr = InWebBrowser;
    UE_LOG(LogTemp, Log, TEXT("FUnrealMCPWebBridge_Simple: WebBrowser reference set"));
}

void FUnrealMCPWebBridge_Simple::ExecuteJavaScript(const FString& Script)
{
    if (TSharedPtr<SWebBrowser> WebBrowser = WebBrowserPtr.Pin())
    {
        WebBrowser->ExecuteJavascript(Script);
        UE_LOG(LogTemp, VeryVerbose, TEXT("FUnrealMCPWebBridge_Simple: Executed JavaScript: %s"), *Script);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FUnrealMCPWebBridge_Simple: Cannot execute JavaScript - WebBrowser is invalid"));
    }
}

void FUnrealMCPWebBridge_Simple::SendMessageToWeb(const FString& Message)
{
    FString Script = FString::Printf(TEXT("if(window.unrealMCP && window.unrealMCP.receiveMessage) { window.unrealMCP.receiveMessage('%s'); }"), *Message.ReplaceCharWithEscapedChar());
    ExecuteJavaScript(Script);
}

void FUnrealMCPWebBridge_Simple::SendScreenshotToWeb(const FString& FilePath)
{
    FString Script = FString::Printf(TEXT("if(window.unrealMCP && window.unrealMCP.receiveScreenshot) { window.unrealMCP.receiveScreenshot('%s'); }"), *FilePath.ReplaceCharWithEscapedChar());
    ExecuteJavaScript(Script);
}

void FUnrealMCPWebBridge_Simple::LogMessage(const FString& Message, const FString& InLogLevel)
{
    if (InLogLevel == TEXT("Error"))
    {
        UE_LOG(LogTemp, Error, TEXT("WebBridge: %s"), *Message);
    }
    else if (InLogLevel == TEXT("Warning"))
    {
        UE_LOG(LogTemp, Warning, TEXT("WebBridge: %s"), *Message);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("WebBridge: %s"), *Message);
    }
}