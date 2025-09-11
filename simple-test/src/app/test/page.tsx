'use client';

import { useState } from 'react';

export default function TestPage() {
  const [tcpResult, setTcpResult] = useState<string>('');
  const [geminiResult, setGeminiResult] = useState<string>('');
  const [loading, setLoading] = useState<string>('');

  const testTCP = async () => {
    setLoading('tcp');
    try {
      const response = await fetch('/api/test-tcp', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          command: 'take_highresshot',
          params: { resolution_multiplier: 1.0 }
        })
      });
      
      const result = await response.json();
      setTcpResult(JSON.stringify(result, null, 2));
    } catch (error) {
      setTcpResult(`Error: ${error}`);
    } finally {
      setLoading('');
    }
  };

  const testGemini = async () => {
    setLoading('gemini');
    try {
      const response = await fetch('/api/test-gemini', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          prompt: 'Hello! Are you working properly?'
        })
      });
      
      const result = await response.json();
      setGeminiResult(JSON.stringify(result, null, 2));
    } catch (error) {
      setGeminiResult(`Error: ${error}`);
    } finally {
      setLoading('');
    }
  };

  return (
    <div className="min-h-screen bg-gray-100 p-8">
      <div className="max-w-4xl mx-auto">
        <h1 className="text-3xl font-bold mb-8 text-center">
          NextJS → Unreal/Gemini Test
        </h1>
        
        <div className="grid grid-cols-1 md:grid-cols-2 gap-8">
          {/* TCP Test */}
          <div className="bg-white p-6 rounded-lg shadow">
            <h2 className="text-xl font-semibold mb-4">TCP Connection Test</h2>
            <p className="text-gray-600 mb-4">
              Test direct TCP connection to Unreal Engine (port 55557)
            </p>
            
            <button
              onClick={testTCP}
              disabled={loading === 'tcp'}
              className="bg-blue-500 hover:bg-blue-600 disabled:bg-gray-400 text-white px-4 py-2 rounded mb-4"
            >
              {loading === 'tcp' ? 'Testing...' : 'Test TCP Connection'}
            </button>
            
            <div className="bg-gray-50 p-4 rounded min-h-[200px]">
              <pre className="text-sm overflow-auto whitespace-pre-wrap">
                {tcpResult || 'No result yet...'}
              </pre>
            </div>
          </div>

          {/* Gemini Test */}
          <div className="bg-white p-6 rounded-lg shadow">
            <h2 className="text-xl font-semibold mb-4">Gemini API Test</h2>
            <p className="text-gray-600 mb-4">
              Test direct Gemini API connection from NextJS
            </p>
            
            <button
              onClick={testGemini}
              disabled={loading === 'gemini'}
              className="bg-green-500 hover:bg-green-600 disabled:bg-gray-400 text-white px-4 py-2 rounded mb-4"
            >
              {loading === 'gemini' ? 'Testing...' : 'Test Gemini API'}
            </button>
            
            <div className="bg-gray-50 p-4 rounded min-h-[200px]">
              <pre className="text-sm overflow-auto whitespace-pre-wrap">
                {geminiResult || 'No result yet...'}
              </pre>
            </div>
          </div>
        </div>

        <div className="mt-8 bg-yellow-50 p-6 rounded-lg">
          <h3 className="text-lg font-semibold mb-2">테스트 목적:</h3>
          <ul className="list-disc list-inside space-y-1 text-gray-700">
            <li>NextJS API Routes에서 TCP 소켓 통신이 안정적으로 작동하는가?</li>
            <li>Gemini API 직접 호출이 문제없이 작동하는가?</li>
            <li>두 기능이 모두 작동한다면 Python MCP 서버 없이도 가능!</li>
          </ul>
        </div>
      </div>
    </div>
  );
}