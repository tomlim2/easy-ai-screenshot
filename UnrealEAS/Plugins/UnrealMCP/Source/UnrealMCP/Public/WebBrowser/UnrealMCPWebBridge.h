#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Engine.h"
#include "UnrealMCPWebBridge.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebMessageReceived, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScreenshotRequested, const FString&, FilePath, const FString&, Prompt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImageProcessed, const FString&, ProcessedImageData);

class UUnrealMCPScreenshotHandler;

/**
 * Bridge object for communication between Unreal Engine and JavaScript
 * This object is exposed to JavaScript in the web browser
 */
UCLASS(BlueprintType, Blueprintable)
class UNREALMCP_API UUnrealMCPWebBridge : public UObject
{
    GENERATED_BODY()

public:
    UUnrealMCPWebBridge();

    /** Called from JavaScript to send a message to Unreal Engine */
    UFUNCTION(BlueprintCallable, Category = "Unreal MCP Web Bridge")
    void ReceiveMessageFromWeb(const FString& Message);

    /** Called from JavaScript to request screenshot processing */
    UFUNCTION(BlueprintCallable, Category = "Unreal MCP Web Bridge")
    void RequestScreenshotProcessing(const FString& FilePath, const FString& Prompt, const FString& StyleParams);

    /** Called from JavaScript to notify that image processing is complete */
    UFUNCTION(BlueprintCallable, Category = "Unreal MCP Web Bridge")
    void NotifyImageProcessingComplete(const FString& ProcessedImageData);

    /** Called from JavaScript to log messages */
    UFUNCTION(BlueprintCallable, Category = "Unreal MCP Web Bridge")
    void LogMessage(const FString& Message, const FString& InLogLevel);

    /** Called from JavaScript to check if Unreal is ready */
    UFUNCTION(BlueprintCallable, Category = "Unreal MCP Web Bridge")
    bool IsUnrealReady();

    /** Send a message from Unreal to JavaScript */
    UFUNCTION(BlueprintCallable, Category = "Unreal MCP Web Bridge")
    void SendMessageToWeb(const FString& Message);

    /** Send screenshot data to web application */
    UFUNCTION(BlueprintCallable, Category = "Unreal MCP Web Bridge")
    void SendScreenshotToWeb(const FString& FilePath);

    /** Notify web that screenshot capture is complete */
    UFUNCTION(BlueprintCallable, Category = "Unreal MCP Web Bridge")
    void NotifyScreenshotCaptured(const FString& FilePath);

    /** Get reference to the web browser widget */
    void SetWebBrowser(TSharedPtr<class SWebBrowser> InWebBrowser);

public:
    /** Event fired when a message is received from the web */
    UPROPERTY(BlueprintAssignable)
    FOnWebMessageReceived OnWebMessageReceived;

    /** Event fired when a screenshot is requested from the web */
    UPROPERTY(BlueprintAssignable)
    FOnScreenshotRequested OnScreenshotRequested;

    /** Event fired when image processing is complete */
    UPROPERTY(BlueprintAssignable)
    FOnImageProcessed OnImageProcessed;

private:
    /** Reference to the web browser widget */
    TWeakPtr<class SWebBrowser> WebBrowserPtr;

    /** Screenshot handler for managing screenshot capture and processing */
    UPROPERTY()
    TObjectPtr<UUnrealMCPScreenshotHandler> ScreenshotHandler;

    /** Execute JavaScript in the web browser */
    void ExecuteJavaScript(const FString& Script);
};