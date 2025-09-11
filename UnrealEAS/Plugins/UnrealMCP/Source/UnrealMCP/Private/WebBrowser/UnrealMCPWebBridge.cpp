#include "WebBrowser/UnrealMCPWebBridge.h"
#include "WebBrowser/UnrealMCPScreenshotHandler.h"
#include "SWebBrowser.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "Json.h"

UUnrealMCPWebBridge::UUnrealMCPWebBridge()
{
    // Initialize screenshot handler as nullptr - will be created when needed
    ScreenshotHandler = nullptr;
}

void UUnrealMCPWebBridge::ReceiveMessageFromWeb(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("UUnrealMCPWebBridge: Received message from web: %s"), *Message);
    
    // Broadcast the message to any listeners
    OnWebMessageReceived.Broadcast(Message);
    
    // Parse JSON messages for specific commands
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);
    
    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        FString Command;
        // Skip JSON parsing for now to avoid errors
        UE_LOG(LogTemp, Log, TEXT("UUnrealMCPWebBridge: JSON message processing temporarily disabled"));
    }
}

void UUnrealMCPWebBridge::RequestScreenshotProcessing(const FString& FilePath, const FString& Prompt, const FString& StyleParams)
{
    UE_LOG(LogTemp, Log, TEXT("UUnrealMCPWebBridge: Screenshot processing requested - File: %s, Prompt: %s"), *FilePath, *Prompt);
    
    // Combine prompt and style params
    FString FullPrompt = FString::Printf(TEXT("%s|%s"), *Prompt, *StyleParams);
    OnScreenshotRequested.Broadcast(FilePath, FullPrompt);
}

void UUnrealMCPWebBridge::NotifyImageProcessingComplete(const FString& ProcessedImageData)
{
    UE_LOG(LogTemp, Log, TEXT("UUnrealMCPWebBridge: Image processing complete, data length: %d"), ProcessedImageData.Len());
    
    OnImageProcessed.Broadcast(ProcessedImageData);
}

void UUnrealMCPWebBridge::LogMessage(const FString& Message, const FString& InLogLevel)
{
    if (InLogLevel == TEXT("error"))
    {
        UE_LOG(LogTemp, Error, TEXT("WebBrowser: %s"), *Message);
    }
    else if (InLogLevel == TEXT("warning"))
    {
        UE_LOG(LogTemp, Warning, TEXT("WebBrowser: %s"), *Message);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("WebBrowser: %s"), *Message);
    }
}

bool UUnrealMCPWebBridge::IsUnrealReady()
{
    // Check if Unreal Engine is ready to receive commands
    return GEngine != nullptr && GEngine->IsInitialized();
}

void UUnrealMCPWebBridge::SendMessageToWeb(const FString& Message)
{
    FString JavaScriptCommand = FString::Printf(TEXT("if(window.unrealMCP && window.unrealMCP.receiveMessage) { window.unrealMCP.receiveMessage('%s'); }"), *Message.Replace(TEXT("'"), TEXT("\\'")));
    ExecuteJavaScript(JavaScriptCommand);
}

void UUnrealMCPWebBridge::SendScreenshotToWeb(const FString& FilePath)
{
    // Escape file path for JavaScript
    FString EscapedPath = FilePath.Replace(TEXT("\\"), TEXT("\\\\")).Replace(TEXT("'"), TEXT("\\'"));
    FString JavaScriptCommand = FString::Printf(TEXT("if(window.unrealMCP && window.unrealMCP.receiveScreenshot) { window.unrealMCP.receiveScreenshot('%s'); }"), *EscapedPath);
    ExecuteJavaScript(JavaScriptCommand);
}

void UUnrealMCPWebBridge::NotifyScreenshotCaptured(const FString& FilePath)
{
    UE_LOG(LogTemp, Log, TEXT("UUnrealMCPWebBridge: Screenshot captured: %s"), *FilePath);
    SendScreenshotToWeb(FilePath);
}

void UUnrealMCPWebBridge::SetWebBrowser(TSharedPtr<SWebBrowser> InWebBrowser)
{
    WebBrowserPtr = InWebBrowser;
}

void UUnrealMCPWebBridge::ExecuteJavaScript(const FString& Script)
{
    if (auto WebBrowser = WebBrowserPtr.Pin())
    {
        WebBrowser->ExecuteJavascript(Script);
        UE_LOG(LogTemp, VeryVerbose, TEXT("UUnrealMCPWebBridge: Executed JavaScript: %s"), *Script);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UUnrealMCPWebBridge: Cannot execute JavaScript - WebBrowser is invalid"));
    }
}