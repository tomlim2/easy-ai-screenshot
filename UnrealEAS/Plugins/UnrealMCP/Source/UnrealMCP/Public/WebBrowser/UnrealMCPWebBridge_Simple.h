#pragma once

#include "CoreMinimal.h"

class SWebBrowser;

/**
 * Simple Bridge for communication between Unreal Engine and JavaScript
 * Non-UObject version to avoid NewObject issues during module initialization
 */
class UNREALMCP_API FUnrealMCPWebBridge_Simple
{
public:
    FUnrealMCPWebBridge_Simple();
    ~FUnrealMCPWebBridge_Simple();

    /** Set the web browser reference */
    void SetWebBrowser(TSharedPtr<SWebBrowser> InWebBrowser);

    /** Execute JavaScript in the web browser */
    void ExecuteJavaScript(const FString& Script);

    /** Send message to web */
    void SendMessageToWeb(const FString& Message);

    /** Send screenshot path to web */
    void SendScreenshotToWeb(const FString& FilePath);

    /** Log message to Unreal console */
    void LogMessage(const FString& Message, const FString& InLogLevel = TEXT("Log"));

private:
    /** Reference to the web browser widget */
    TWeakPtr<SWebBrowser> WebBrowserPtr;
};