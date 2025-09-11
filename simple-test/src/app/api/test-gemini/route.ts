import { NextRequest, NextResponse } from 'next/server';
import { GoogleGenerativeAI } from '@google/generative-ai';

export async function POST(request: NextRequest) {
  try {
    const body = await request.json();
    const { prompt, testImage } = body;

    console.log('Testing Gemini API connection...');
    
    // Get API key from environment or request
    const apiKey = process.env.GOOGLE_API_KEY;
    if (!apiKey) {
      return NextResponse.json({
        success: false,
        error: 'GOOGLE_API_KEY not configured'
      }, { status: 400 });
    }

    const genAI = new GoogleGenerativeAI(apiKey);
    
    if (testImage) {
      // Test image generation/editing
      const model = genAI.getGenerativeModel({ model: "gemini-2.5-flash" });
      
      const result = await model.generateContent([
        "Transform this description into a visual style prompt: " + prompt
      ]);
      
      return NextResponse.json({
        success: true,
        message: 'Gemini text generation successful',
        result: result.response.text()
      });
    } else {
      // Test basic text generation
      const model = genAI.getGenerativeModel({ model: "gemini-2.5-flash" });
      
      const result = await model.generateContent(prompt || "Hello, are you working?");
      
      return NextResponse.json({
        success: true,
        message: 'Gemini API connection successful',
        result: result.response.text()
      });
    }
  } catch (error) {
    console.error('Gemini API test error:', error);
    return NextResponse.json({
      success: false,
      error: error instanceof Error ? error.message : 'Unknown error'
    }, { status: 500 });
  }
}