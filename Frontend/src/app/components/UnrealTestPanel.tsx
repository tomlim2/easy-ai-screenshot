'use client';

import { useState, useEffect } from 'react';
import { unrealBridge, type ScreenshotProcessingRequest } from '../utils/unrealBridge';

interface UnrealTestPanelProps {
  className?: string;
}

export default function UnrealTestPanel({ className }: UnrealTestPanelProps) {
  const [isUnrealConnected, setIsUnrealConnected] = useState(false);
  const [lastScreenshotPath, setLastScreenshotPath] = useState<string>('');
  const [testMessage, setTestMessage] = useState('');
  const [prompt, setPrompt] = useState('Transform this screenshot into a beautiful artistic style');
  const [styleParams, setStyleParams] = useState('watercolor, soft lighting, artistic');

  useEffect(() => {
    // Check Unreal connection status
    const checkConnection = () => {
      const connected = unrealBridge.isUnrealReady();
      setIsUnrealConnected(connected);
    };

    checkConnection();
    const interval = setInterval(checkConnection, 2000);

    // Set up screenshot handler
    unrealBridge.onScreenshotFromUnreal((filePath) => {
      setLastScreenshotPath(filePath);
      console.log('Screenshot received in test panel:', filePath);
    });

    return () => clearInterval(interval);
  }, []);

  const handleSendTestMessage = () => {
    if (!testMessage.trim()) return;
    
    const message = JSON.stringify({
      type: 'test',
      data: { message: testMessage, timestamp: Date.now() }
    });
    
    unrealBridge.sendMessageToUnreal(message);
    unrealBridge.logToUnreal(`Test message sent: ${testMessage}`, 'log');
    setTestMessage('');
  };

  const handleRequestScreenshot = () => {
    // Request a screenshot capture from Unreal
    const message = JSON.stringify({
      type: 'capture_screenshot',
      data: { prompt, timestamp: Date.now() }
    });
    
    unrealBridge.sendMessageToUnreal(message);
    unrealBridge.logToUnreal('Screenshot capture requested', 'log');
  };

  const handleProcessScreenshot = () => {
    if (!lastScreenshotPath) {
      alert('No screenshot available. Please capture a screenshot first.');
      return;
    }

    const request: ScreenshotProcessingRequest = {
      filePath: lastScreenshotPath,
      prompt: prompt,
      styleParams: styleParams
    };

    unrealBridge.requestScreenshotProcessing(request);
    unrealBridge.logToUnreal('Screenshot processing requested', 'log');
  };

  const handleSimulateProcessingComplete = () => {
    // Simulate processing completion with dummy data
    const dummyImageData = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNkYPhfDwAChwGA60e6kgAAAABJRU5ErkJggg==';
    unrealBridge.notifyImageProcessingComplete(dummyImageData);
    unrealBridge.logToUnreal('Simulated processing completion', 'log');
  };

  return (
    <div className={`p-6 bg-gray-50 border rounded-lg ${className}`}>
      <h2 className="text-xl font-bold mb-4">Unreal Engine Connection Test</h2>
      
      {/* Connection Status */}
      <div className="mb-4">
        <div className="flex items-center gap-2">
          <div className={`w-3 h-3 rounded-full ${isUnrealConnected ? 'bg-green-500' : 'bg-red-500'}`}></div>
          <span className={`font-medium ${isUnrealConnected ? 'text-green-700' : 'text-red-700'}`}>
            {isUnrealConnected ? 'Connected to Unreal Engine' : 'Not connected to Unreal Engine'}
          </span>
        </div>
        {!isUnrealConnected && (
          <p className="text-sm text-gray-600 mt-1">
            Open this app in the Unreal Engine web browser tab to establish connection.
          </p>
        )}
      </div>

      {/* Test Message */}
      <div className="mb-4">
        <label className="block text-sm font-medium mb-2">Send Test Message to Unreal:</label>
        <div className="flex gap-2">
          <input
            type="text"
            value={testMessage}
            onChange={(e) => setTestMessage(e.target.value)}
            placeholder="Enter a test message..."
            className="flex-1 px-3 py-2 border border-gray-300 rounded-md"
            disabled={!isUnrealConnected}
          />
          <button
            onClick={handleSendTestMessage}
            disabled={!isUnrealConnected || !testMessage.trim()}
            className="px-4 py-2 bg-blue-500 text-white rounded-md hover:bg-blue-600 disabled:bg-gray-300"
          >
            Send
          </button>
        </div>
      </div>

      {/* Screenshot Controls */}
      <div className="space-y-4">
        <h3 className="text-lg font-semibold">Screenshot Controls</h3>
        
        {/* Prompt Input */}
        <div>
          <label className="block text-sm font-medium mb-2">AI Prompt:</label>
          <input
            type="text"
            value={prompt}
            onChange={(e) => setPrompt(e.target.value)}
            placeholder="Enter AI processing prompt..."
            className="w-full px-3 py-2 border border-gray-300 rounded-md"
          />
        </div>

        {/* Style Parameters */}
        <div>
          <label className="block text-sm font-medium mb-2">Style Parameters:</label>
          <input
            type="text"
            value={styleParams}
            onChange={(e) => setStyleParams(e.target.value)}
            placeholder="Enter style parameters..."
            className="w-full px-3 py-2 border border-gray-300 rounded-md"
          />
        </div>

        {/* Action Buttons */}
        <div className="flex gap-2 flex-wrap">
          <button
            onClick={handleRequestScreenshot}
            disabled={!isUnrealConnected}
            className="px-4 py-2 bg-green-500 text-white rounded-md hover:bg-green-600 disabled:bg-gray-300"
          >
            Request Screenshot
          </button>
          
          <button
            onClick={handleProcessScreenshot}
            disabled={!isUnrealConnected || !lastScreenshotPath}
            className="px-4 py-2 bg-orange-500 text-white rounded-md hover:bg-orange-600 disabled:bg-gray-300"
          >
            Process Screenshot
          </button>
          
          <button
            onClick={handleSimulateProcessingComplete}
            disabled={!isUnrealConnected}
            className="px-4 py-2 bg-purple-500 text-white rounded-md hover:bg-purple-600 disabled:bg-gray-300"
          >
            Simulate Complete
          </button>
        </div>

        {/* Screenshot Status */}
        {lastScreenshotPath && (
          <div className="mt-4 p-3 bg-blue-50 border border-blue-200 rounded-md">
            <p className="text-sm font-medium text-blue-800">Last Screenshot:</p>
            <p className="text-sm text-blue-600 break-all">{lastScreenshotPath}</p>
          </div>
        )}
      </div>

      {/* Instructions */}
      <div className="mt-6 p-4 bg-yellow-50 border border-yellow-200 rounded-md">
        <h4 className="text-sm font-medium text-yellow-800 mb-2">Instructions:</h4>
        <ul className="text-sm text-yellow-700 space-y-1">
          <li>1. Open this app in Unreal Engine's AI Screenshot Tool tab</li>
          <li>2. Use "Request Screenshot" to trigger screenshot capture in Unreal</li>
          <li>3. Use "Process Screenshot" to send the captured image to AI processing</li>
          <li>4. Check Unreal Engine's output log for communication messages</li>
        </ul>
      </div>
    </div>
  );
}