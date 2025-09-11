/**
 * Unreal Engine Bridge for JavaScript communication
 * Handles communication between NextJS and Unreal Engine web browser
 */

// Declare the global unreal bridge object
declare global {
  interface Window {
    unrealMCP: {
      receiveMessage?: (message: string) => void;
      receiveScreenshot?: (filePath: string) => void;
    };
    unrealMCPBridge?: {
      ReceiveMessageFromWeb: (message: string) => void;
      RequestScreenshotProcessing: (filePath: string, prompt: string, styleParams: string) => void;
      NotifyImageProcessingComplete: (processedImageData: string) => void;
      LogMessage: (message: string, logLevel: string) => void;
      IsUnrealReady: () => boolean;
    };
  }
}

export interface ScreenshotProcessingRequest {
  filePath: string;
  prompt: string;
  styleParams: string;
}

export interface ProcessedImageResult {
  success: boolean;
  imageData?: string;
  error?: string;
}

export class UnrealBridge {
  private static instance: UnrealBridge;
  private messageHandlers: Map<string, (data: any) => void> = new Map();
  private screenshotHandlers: ((filePath: string) => void)[] = [];
  
  private constructor() {
    this.initializeBridge();
  }

  public static getInstance(): UnrealBridge {
    if (!UnrealBridge.instance) {
      UnrealBridge.instance = new UnrealBridge();
    }
    return UnrealBridge.instance;
  }

  private initializeBridge(): void {
    // Initialize the bridge object
    window.unrealMCP = {
      receiveMessage: (message: string) => {
        this.handleMessageFromUnreal(message);
      },
      receiveScreenshot: (filePath: string) => {
        this.handleScreenshotFromUnreal(filePath);
      }
    };

    console.log('Unreal MCP Bridge initialized on NextJS side');
  }

  /**
   * Check if Unreal Engine bridge is available
   */
  public isUnrealReady(): boolean {
    try {
      return window.unrealMCPBridge?.IsUnrealReady?.() ?? false;
    } catch (error) {
      console.warn('Unreal bridge not available:', error);
      return false;
    }
  }

  /**
   * Send a message to Unreal Engine
   */
  public sendMessageToUnreal(message: string): void {
    try {
      if (window.unrealMCPBridge?.ReceiveMessageFromWeb) {
        window.unrealMCPBridge.ReceiveMessageFromWeb(message);
        console.log('Sent message to Unreal:', message);
      } else {
        console.warn('Unreal bridge not available - cannot send message');
      }
    } catch (error) {
      console.error('Error sending message to Unreal:', error);
    }
  }

  /**
   * Request screenshot processing from Unreal Engine
   */
  public requestScreenshotProcessing(request: ScreenshotProcessingRequest): void {
    try {
      if (window.unrealMCPBridge?.RequestScreenshotProcessing) {
        window.unrealMCPBridge.RequestScreenshotProcessing(
          request.filePath,
          request.prompt,
          request.styleParams
        );
        console.log('Requested screenshot processing:', request);
      } else {
        console.warn('Unreal bridge not available - cannot request screenshot processing');
      }
    } catch (error) {
      console.error('Error requesting screenshot processing:', error);
    }
  }

  /**
   * Notify Unreal Engine that image processing is complete
   */
  public notifyImageProcessingComplete(processedImageData: string): void {
    try {
      if (window.unrealMCPBridge?.NotifyImageProcessingComplete) {
        window.unrealMCPBridge.NotifyImageProcessingComplete(processedImageData);
        console.log('Notified Unreal of image processing completion');
      } else {
        console.warn('Unreal bridge not available - cannot notify completion');
      }
    } catch (error) {
      console.error('Error notifying image processing completion:', error);
    }
  }

  /**
   * Log a message to Unreal Engine console
   */
  public logToUnreal(message: string, level: 'log' | 'warning' | 'error' = 'log'): void {
    try {
      if (window.unrealMCPBridge?.LogMessage) {
        window.unrealMCPBridge.LogMessage(message, level);
      }
    } catch (error) {
      console.error('Error logging to Unreal:', error);
    }
  }

  /**
   * Register a handler for messages from Unreal Engine
   */
  public onMessageFromUnreal(type: string, handler: (data: any) => void): void {
    this.messageHandlers.set(type, handler);
  }

  /**
   * Register a handler for screenshot notifications from Unreal Engine
   */
  public onScreenshotFromUnreal(handler: (filePath: string) => void): void {
    this.screenshotHandlers.push(handler);
  }

  private handleMessageFromUnreal(message: string): void {
    console.log('Received message from Unreal:', message);
    
    try {
      const parsed = JSON.parse(message);
      const handler = this.messageHandlers.get(parsed.type);
      if (handler) {
        handler(parsed.data);
      }
    } catch (error) {
      console.error('Error parsing message from Unreal:', error);
    }
  }

  private handleScreenshotFromUnreal(filePath: string): void {
    console.log('Received screenshot from Unreal:', filePath);
    
    this.screenshotHandlers.forEach(handler => {
      try {
        handler(filePath);
      } catch (error) {
        console.error('Error in screenshot handler:', error);
      }
    });
  }
}

// Export singleton instance
export const unrealBridge = UnrealBridge.getInstance();

// Export type definitions
export type { ScreenshotProcessingRequest, ProcessedImageResult };