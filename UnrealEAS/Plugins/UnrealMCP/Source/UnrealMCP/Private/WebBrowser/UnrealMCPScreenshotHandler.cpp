#include "WebBrowser/UnrealMCPScreenshotHandler.h"
#include "WebBrowser/UnrealMCPWebBridge.h"
#include "Commands/UnrealMCPRenderingCommands.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"
#include "Misc/Base64.h"
#include "Engine/Engine.h"

UUnrealMCPScreenshotHandler::UUnrealMCPScreenshotHandler()
{
    // Initialize rendering commands
    RenderingCommands = MakeShared<FUnrealMCPRenderingCommands>();
}

void UUnrealMCPScreenshotHandler::Initialize(UUnrealMCPWebBridge* InWebBridge)
{
    WebBridge = InWebBridge;
    
    if (WebBridge)
    {
        // Bind to web bridge events
        WebBridge->OnScreenshotRequested.AddDynamic(this, &UUnrealMCPScreenshotHandler::HandleScreenshotRequest);
        WebBridge->OnImageProcessed.AddDynamic(this, &UUnrealMCPScreenshotHandler::HandleImageProcessingComplete);
        
        UE_LOG(LogTemp, Log, TEXT("UUnrealMCPScreenshotHandler: Initialized and bound to web bridge"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UUnrealMCPScreenshotHandler: Failed to initialize - WebBridge is null"));
    }
}

void UUnrealMCPScreenshotHandler::HandleScreenshotRequest(const FString& FilePath, const FString& Prompt)
{
    UE_LOG(LogTemp, Log, TEXT("UUnrealMCPScreenshotHandler: Screenshot request received - File: %s, Prompt: %s"), *FilePath, *Prompt);
    
    // If a file path is provided, use it; otherwise capture a new screenshot
    if (!FilePath.IsEmpty() && FPaths::FileExists(FilePath))
    {
        LastScreenshotPath = FilePath;
        
        // Notify web that screenshot is ready for processing
        if (WebBridge)
        {
            WebBridge->NotifyScreenshotCaptured(LastScreenshotPath);
        }
    }
    else
    {
        // Capture a new screenshot
        CaptureScreenshot(Prompt);
    }
}

void UUnrealMCPScreenshotHandler::HandleImageProcessingComplete(const FString& ProcessedImageData)
{
    UE_LOG(LogTemp, Log, TEXT("UUnrealMCPScreenshotHandler: Image processing complete, data length: %d"), ProcessedImageData.Len());
    
    if (!ProcessedImageData.IsEmpty())
    {
        // Generate output filename
        FString OutputPath = GenerateScreenshotFilename();
        OutputPath = FPaths::ChangeExtension(OutputPath, TEXT("_processed.png"));
        
        // Save the processed image
        if (SaveProcessedImage(ProcessedImageData, OutputPath))
        {
            UE_LOG(LogTemp, Log, TEXT("UUnrealMCPScreenshotHandler: Processed image saved to: %s"), *OutputPath);
            
            // TODO: Create texture and material from the processed image
            // This can be implemented in a future update
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UUnrealMCPScreenshotHandler: Failed to save processed image"));
        }
    }
}

void UUnrealMCPScreenshotHandler::CaptureScreenshot(const FString& Prompt, float ResolutionMultiplier)
{
    if (!RenderingCommands.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("UUnrealMCPScreenshotHandler: RenderingCommands is not valid"));
        return;
    }
    
    // Generate filename for the screenshot
    LastScreenshotPath = GenerateScreenshotFilename();
    
    // Prepare parameters for the screenshot command
    TSharedPtr<FJsonObject> Params = MakeShareable(new FJsonObject);
    Params->SetNumberField(TEXT("resolution_multiplier"), ResolutionMultiplier);
    Params->SetBoolField(TEXT("include_ui"), false);
    Params->SetStringField(TEXT("filename"), FPaths::GetBaseFilename(LastScreenshotPath));
    
    // Execute the screenshot command
    TSharedPtr<FJsonObject> Result = RenderingCommands->HandleCommand(TEXT("take_highresshot"), Params);
    
    if (Result.IsValid())
    {
        bool bSuccess = false;
        Result->TryGetBoolField(TEXT("success"), bSuccess);
        
        if (bSuccess)
        {
            UE_LOG(LogTemp, Log, TEXT("UUnrealMCPScreenshotHandler: Screenshot captured successfully"));
            
            // Notify web bridge about the screenshot
            if (WebBridge)
            {
                WebBridge->NotifyScreenshotCaptured(LastScreenshotPath);
            }
        }
        else
        {
            FString ErrorMessage;
            Result->TryGetStringField(TEXT("error"), ErrorMessage);
            UE_LOG(LogTemp, Error, TEXT("UUnrealMCPScreenshotHandler: Screenshot failed: %s"), *ErrorMessage);
        }
    }
}

FString UUnrealMCPScreenshotHandler::GenerateScreenshotFilename()
{
    // Generate a unique filename based on timestamp
    FDateTime Now = FDateTime::Now();
    FString Timestamp = Now.ToString(TEXT("%Y%m%d_%H%M%S"));
    
    // Use the project's screenshot directory
    FString ScreenshotDir = FPaths::ProjectDir() / TEXT("Screenshots");
    
    // Ensure directory exists
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*ScreenshotDir))
    {
        PlatformFile.CreateDirectoryTree(*ScreenshotDir);
    }
    
    FString Filename = FString::Printf(TEXT("AIScreenshot_%s.png"), *Timestamp);
    return ScreenshotDir / Filename;
}

bool UUnrealMCPScreenshotHandler::SaveProcessedImage(const FString& ImageData, const FString& OutputPath)
{
    // Check if this is Base64 data
    if (ImageData.StartsWith(TEXT("data:image/")))
    {
        // Extract Base64 data after the comma
        int32 CommaIndex;
        if (ImageData.FindChar(',', CommaIndex))
        {
            FString Base64Data = ImageData.Mid(CommaIndex + 1);
            return SaveBase64AsImage(Base64Data, OutputPath);
        }
    }
    else if (ImageData.StartsWith(TEXT("iVBOR")) || ImageData.StartsWith(TEXT("/9j/"))) // PNG or JPEG Base64
    {
        return SaveBase64AsImage(ImageData, OutputPath);
    }
    else if (ImageData.StartsWith(TEXT("http://")) || ImageData.StartsWith(TEXT("https://")))
    {
        // This is a URL - we would need to download it
        UE_LOG(LogTemp, Warning, TEXT("UUnrealMCPScreenshotHandler: URL-based image data not yet supported: %s"), *ImageData);
        return false;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("UUnrealMCPScreenshotHandler: Unknown image data format"));
    return false;
}

bool UUnrealMCPScreenshotHandler::SaveBase64AsImage(const FString& Base64Data, const FString& FilePath)
{
    // Decode Base64 data
    TArray<uint8> ImageBytes;
    if (!FBase64::Decode(Base64Data, ImageBytes))
    {
        UE_LOG(LogTemp, Error, TEXT("UUnrealMCPScreenshotHandler: Failed to decode Base64 image data"));
        return false;
    }
    
    // Save to file
    if (!FFileHelper::SaveArrayToFile(ImageBytes, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("UUnrealMCPScreenshotHandler: Failed to save image to file: %s"), *FilePath);
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("UUnrealMCPScreenshotHandler: Successfully saved image to: %s"), *FilePath);
    return true;
}