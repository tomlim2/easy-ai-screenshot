import { NextRequest, NextResponse } from 'next/server';
import net from 'net';

interface UnrealCommand {
  command: string;
  params: Record<string, unknown>;
}

interface UnrealResponse {
  status?: string;
  success?: boolean;
  message?: string;
  [key: string]: unknown;
}

export async function POST(request: NextRequest) {
  try {
    const body = await request.json();
    const { command, params } = body;

    console.log('Testing TCP connection to Unreal Engine...');
    
    // Test TCP connection to Unreal Engine
    const result = await testUnrealTCP(command, params);
    
    return NextResponse.json({
      success: true,
      message: 'TCP connection test successful',
      result: result
    });
  } catch (error) {
    console.error('TCP test error:', error);
    return NextResponse.json({
      success: false,
      error: error instanceof Error ? error.message : 'Unknown error'
    }, { status: 500 });
  }
}

async function testUnrealTCP(command: string, params: Record<string, unknown>): Promise<UnrealResponse> {
  return new Promise((resolve, reject) => {
    const client = new net.Socket();
    let responseData = '';
    
    // Set timeout - longer for screenshot commands
    const timeoutDuration = command === 'take_highresshot' ? 30000 : 10000;
    const timeout = setTimeout(() => {
      client.destroy();
      reject(new Error(`TCP connection timeout after ${timeoutDuration}ms`));
    }, timeoutDuration);
    
    client.connect(55557, '127.0.0.1', () => {
      console.log('Connected to Unreal Engine');
      
      // Send command in Unreal's expected format (no newline, use "type" field)
      const message = JSON.stringify({
        type: command,
        params: params || {}
      });
      
      console.log('Sending command:', message);
      client.write(message);
    });
    
    client.on('data', (data) => {
      responseData += data.toString();
      
      // Check if we have a complete JSON response
      try {
        const response = JSON.parse(responseData);
        clearTimeout(timeout);
        client.destroy();
        resolve(response);
      } catch {
        // Still receiving data, wait for more
      }
    });
    
    client.on('error', (error) => {
      clearTimeout(timeout);
      reject(error);
    });
    
    client.on('close', () => {
      clearTimeout(timeout);
      if (responseData) {
        try {
          const response = JSON.parse(responseData);
          resolve(response);
        } catch {
          reject(new Error('Invalid response from Unreal Engine'));
        }
      } else {
        reject(new Error('No response from Unreal Engine'));
      }
    });
  });
}