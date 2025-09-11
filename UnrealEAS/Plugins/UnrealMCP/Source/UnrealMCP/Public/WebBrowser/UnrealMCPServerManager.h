#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/PlatformProcess.h"
#include "UnrealMCPServerManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerStatusChanged, bool, bIsRunning);

/**
 * Manager for NextJS server process
 * Handles starting, stopping, and monitoring the NextJS development server
 */
UCLASS(BlueprintType)
class UNREALMCP_API UUnrealMCPServerManager : public UObject
{
    GENERATED_BODY()

public:
    UUnrealMCPServerManager();
    virtual ~UUnrealMCPServerManager();

    /** Start the NextJS server if it's not already running */
    UFUNCTION(BlueprintCallable, Category = "Server Manager")
    bool StartNextJSServer();

    /** Stop the NextJS server */
    UFUNCTION(BlueprintCallable, Category = "Server Manager")
    void StopNextJSServer();

    /** Check if the NextJS server is running */
    UFUNCTION(BlueprintCallable, Category = "Server Manager")
    bool IsServerRunning() const;

    /** Check if localhost:3000 is responding */
    UFUNCTION(BlueprintCallable, Category = "Server Manager")
    void CheckServerStatus();

    /** Get the NextJS project path */
    UFUNCTION(BlueprintCallable, Category = "Server Manager")
    FString GetNextJSProjectPath() const;

    /** Set the NextJS project path */
    UFUNCTION(BlueprintCallable, Category = "Server Manager")
    void SetNextJSProjectPath(const FString& InPath);

    /** Event fired when server status changes */
    UPROPERTY(BlueprintAssignable)
    FOnServerStatusChanged OnServerStatusChanged;

private:
    /** Process handle for the NextJS server */
    FProcHandle ServerProcessHandle;

    /** Path to the NextJS project */
    FString NextJSProjectPath;

    /** Current server status */
    bool bIsServerRunning;

    /** Thread for monitoring server status */
    FRunnableThread* MonitorThread;

    /** Runnable object for server monitoring */
    class FServerMonitorRunnable* ServerMonitor;

    /** Find Node.js executable path */
    FString FindNodeExecutable() const;

    /** Find npm executable path */
    FString FindNpmExecutable() const;

    /** Check if port 3000 is in use */
    bool IsPort3000InUse() const;

    /** Initialize the server monitor */
    void InitializeMonitor();

    /** Shutdown the server monitor */
    void ShutdownMonitor();

public:
    /** Initialize the server manager */
    void Initialize();

    /** Shutdown the server manager */
    void Shutdown();
};

/**
 * Runnable class for monitoring server status in background
 */
class FServerMonitorRunnable : public FRunnable
{
public:
    FServerMonitorRunnable(UUnrealMCPServerManager* InServerManager);
    virtual ~FServerMonitorRunnable();

    // FRunnable interface
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;

private:
    /** Reference to the server manager */
    TWeakObjectPtr<UUnrealMCPServerManager> ServerManager;

    /** Flag to stop the monitoring thread */
    FThreadSafeCounter StopTaskCounter;

    /** Check if server is responding */
    bool CheckServerResponse() const;
};