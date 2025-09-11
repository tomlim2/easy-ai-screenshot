#include "WebBrowser/UnrealMCPServerManager.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Engine/Engine.h"

UUnrealMCPServerManager::UUnrealMCPServerManager()
    : bIsServerRunning(false)
    , MonitorThread(nullptr)
    , ServerMonitor(nullptr)
{
    // Set default NextJS project path (relative to the Unreal project)
    NextJSProjectPath = FPaths::ProjectDir() / TEXT("../Frontend");
}

UUnrealMCPServerManager::~UUnrealMCPServerManager()
{
    Shutdown();
}

void UUnrealMCPServerManager::Initialize()
{
    UE_LOG(LogTemp, Log, TEXT("UUnrealMCPServerManager: Initializing"));
    
    // Normalize the NextJS project path
    NextJSProjectPath = FPaths::ConvertRelativePathToFull(NextJSProjectPath);
    
    // Check if the NextJS project exists
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*NextJSProjectPath))
    {
        UE_LOG(LogTemp, Error, TEXT("UUnrealMCPServerManager: NextJS project directory not found: %s"), *NextJSProjectPath);
        return;
    }
    
    // Check initial server status
    CheckServerStatus();
    
    // Initialize monitoring
    InitializeMonitor();
}

void UUnrealMCPServerManager::Shutdown()
{
    UE_LOG(LogTemp, Log, TEXT("UUnrealMCPServerManager: Shutting down"));
    
    // Stop monitoring
    ShutdownMonitor();
    
    // Stop server if we started it
    StopNextJSServer();
}

bool UUnrealMCPServerManager::StartNextJSServer()
{
    if (bIsServerRunning)
    {
        UE_LOG(LogTemp, Log, TEXT("UUnrealMCPServerManager: Server is already running"));
        return true;
    }
    
    // Check if port is already in use by another process
    if (IsPort3000InUse())
    {
        UE_LOG(LogTemp, Log, TEXT("UUnrealMCPServerManager: Port 3000 is already in use, assuming NextJS is running"));
        bIsServerRunning = true;
        OnServerStatusChanged.Broadcast(true);
        return true;
    }
    
    FString NpmExecutable = FindNpmExecutable();
    if (NpmExecutable.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("UUnrealMCPServerManager: npm executable not found"));
        return false;
    }
    
    // Parameters for npm run dev
    FString Parameters = TEXT("run dev");
    
    UE_LOG(LogTemp, Log, TEXT("UUnrealMCPServerManager: Starting NextJS server"));
    UE_LOG(LogTemp, Log, TEXT("Command: %s %s"), *NpmExecutable, *Parameters);
    UE_LOG(LogTemp, Log, TEXT("Working Directory: %s"), *NextJSProjectPath);
    
    // Start the process
    ServerProcessHandle = FPlatformProcess::CreateProc(
        *NpmExecutable,
        *Parameters,
        false,      // bLaunchDetached
        false,      // bLaunchHidden
        false,      // bLaunchReallyHidden
        nullptr,    // OutProcessID
        0,          // PriorityModifier
        *NextJSProjectPath, // OptionalWorkingDirectory
        nullptr,    // PipeWriteChild
        nullptr     // PipeReadChild
    );
    
    if (ServerProcessHandle.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("UUnrealMCPServerManager: NextJS server process started successfully"));
        bIsServerRunning = true;
        OnServerStatusChanged.Broadcast(true);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UUnrealMCPServerManager: Failed to start NextJS server process"));
        return false;
    }
}

void UUnrealMCPServerManager::StopNextJSServer()
{
    if (ServerProcessHandle.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("UUnrealMCPServerManager: Stopping NextJS server process"));
        FPlatformProcess::TerminateProc(ServerProcessHandle, true);
        FPlatformProcess::WaitForProc(ServerProcessHandle);
        FPlatformProcess::CloseProc(ServerProcessHandle);
        ServerProcessHandle = FProcHandle();
        
        bIsServerRunning = false;
        OnServerStatusChanged.Broadcast(false);
    }
}

bool UUnrealMCPServerManager::IsServerRunning() const
{
    return bIsServerRunning;
}

void UUnrealMCPServerManager::CheckServerStatus()
{
    bool bWasRunning = bIsServerRunning;
    bIsServerRunning = IsPort3000InUse();
    
    if (bWasRunning != bIsServerRunning)
    {
        UE_LOG(LogTemp, Log, TEXT("UUnrealMCPServerManager: Server status changed to: %s"), 
               bIsServerRunning ? TEXT("Running") : TEXT("Stopped"));
        OnServerStatusChanged.Broadcast(bIsServerRunning);
    }
}

FString UUnrealMCPServerManager::GetNextJSProjectPath() const
{
    return NextJSProjectPath;
}

void UUnrealMCPServerManager::SetNextJSProjectPath(const FString& InPath)
{
    NextJSProjectPath = InPath;
}

FString UUnrealMCPServerManager::FindNodeExecutable() const
{
    FString NodePath;
    
#if PLATFORM_WINDOWS
    // Try common Node.js installation paths on Windows
    TArray<FString> PossiblePaths = {
        TEXT("C:/Program Files/nodejs/node.exe"),
        TEXT("C:/Program Files (x86)/nodejs/node.exe"),
        TEXT("node") // Assume it's in PATH
    };
    
    for (const FString& Path : PossiblePaths)
    {
        if (Path == TEXT("node") || FPaths::FileExists(Path))
        {
            NodePath = Path;
            break;
        }
    }
#else
    // For Linux/Mac
    NodePath = TEXT("node");
#endif
    
    return NodePath;
}

FString UUnrealMCPServerManager::FindNpmExecutable() const
{
    FString NpmPath;
    
#if PLATFORM_WINDOWS
    // Try common npm installation paths on Windows
    TArray<FString> PossiblePaths = {
        TEXT("C:/Program Files/nodejs/npm.cmd"),
        TEXT("C:/Program Files (x86)/nodejs/npm.cmd"),
        TEXT("npm.cmd"), // Assume it's in PATH
        TEXT("npm")      // Fallback
    };
    
    for (const FString& Path : PossiblePaths)
    {
        if (Path.Contains(TEXT("npm")) && (Path.EndsWith(TEXT(".cmd")) || Path == TEXT("npm")))
        {
            if (Path == TEXT("npm.cmd") || Path == TEXT("npm") || FPaths::FileExists(Path))
            {
                NpmPath = Path;
                break;
            }
        }
    }
#else
    // For Linux/Mac
    NpmPath = TEXT("npm");
#endif
    
    return NpmPath;
}

bool UUnrealMCPServerManager::IsPort3000InUse() const
{
    // Simple check by trying to create a socket connection
    // This is a basic implementation - a more robust version would use HTTP request
    return false; // For now, we'll implement this later with HTTP module
}

void UUnrealMCPServerManager::InitializeMonitor()
{
    if (!ServerMonitor && !MonitorThread)
    {
        ServerMonitor = new FServerMonitorRunnable(this);
        MonitorThread = FRunnableThread::Create(ServerMonitor, TEXT("ServerMonitorThread"));
        UE_LOG(LogTemp, Log, TEXT("UUnrealMCPServerManager: Server monitor initialized"));
    }
}

void UUnrealMCPServerManager::ShutdownMonitor()
{
    if (MonitorThread && ServerMonitor)
    {
        MonitorThread->Kill(true);
        delete MonitorThread;
        MonitorThread = nullptr;
        ServerMonitor = nullptr; // Thread cleanup will handle deletion
        UE_LOG(LogTemp, Log, TEXT("UUnrealMCPServerManager: Server monitor shutdown"));
    }
}

// FServerMonitorRunnable implementation
FServerMonitorRunnable::FServerMonitorRunnable(UUnrealMCPServerManager* InServerManager)
    : ServerManager(InServerManager)
{
}

FServerMonitorRunnable::~FServerMonitorRunnable()
{
    Stop();
}

bool FServerMonitorRunnable::Init()
{
    UE_LOG(LogTemp, Log, TEXT("FServerMonitorRunnable: Initialized"));
    return true;
}

uint32 FServerMonitorRunnable::Run()
{
    UE_LOG(LogTemp, Log, TEXT("FServerMonitorRunnable: Started monitoring"));
    
    while (StopTaskCounter.GetValue() == 0)
    {
        if (ServerManager.IsValid())
        {
            // Check server status every 5 seconds
            AsyncTask(ENamedThreads::GameThread, [WeakServerManager = ServerManager]()
            {
                if (WeakServerManager.IsValid())
                {
                    WeakServerManager->CheckServerStatus();
                }
            });
        }
        
        // Sleep for 5 seconds
        FPlatformProcess::Sleep(5.0f);
    }
    
    UE_LOG(LogTemp, Log, TEXT("FServerMonitorRunnable: Stopped monitoring"));
    return 0;
}

void FServerMonitorRunnable::Stop()
{
    StopTaskCounter.Increment();
}

void FServerMonitorRunnable::Exit()
{
}

bool FServerMonitorRunnable::CheckServerResponse() const
{
    // This could be implemented with HTTP requests to localhost:3000
    // For now, return false as a placeholder
    return false;
}