#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnrealMCPScreenshotHandler.generated.h"

class FUnrealMCPRenderingCommands;
class UUnrealMCPWebBridge;

/**
 * Handler class that connects the web browser interface with the screenshot system
 * Manages screenshot capture requests from the web and processes the results
 */
UCLASS()
class UNREALMCP_API UUnrealMCPScreenshotHandler : public UObject
{
    GENERATED_BODY()

public:
    UUnrealMCPScreenshotHandler();

    /** Initialize the handler with a web bridge */
    void Initialize(UUnrealMCPWebBridge* InWebBridge);

    /** Handle screenshot request from web */
    UFUNCTION()
    void HandleScreenshotRequest(const FString& FilePath, const FString& Prompt);

    /** Handle image processing completion from web */
    UFUNCTION()
    void HandleImageProcessingComplete(const FString& ProcessedImageData);

    /** Capture a screenshot and notify the web application */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Handler")
    void CaptureScreenshot(const FString& Prompt = TEXT(""), float ResolutionMultiplier = 1.0f);

private:
    /** Reference to the web bridge */
    UPROPERTY()
    TObjectPtr<UUnrealMCPWebBridge> WebBridge;

    /** Rendering commands handler */
    TSharedPtr<FUnrealMCPRenderingCommands> RenderingCommands;

    /** Last captured screenshot file path */
    FString LastScreenshotPath;

    /** Generate a unique filename for screenshots */
    FString GenerateScreenshotFilename();

    /** Save processed image data to file */
    bool SaveProcessedImage(const FString& ImageData, const FString& OutputPath);

    /** Convert Base64 data to image file */
    bool SaveBase64AsImage(const FString& Base64Data, const FString& FilePath);
};