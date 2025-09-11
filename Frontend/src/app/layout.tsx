'use client';

import { Inter, Bodoni_Moda } from "next/font/google";
import ModalProvider from "./components/modal/ModalProvider";
import ToastProvider from "./components/toast/ToastProvider";
import { useEffect } from "react";
import { unrealBridge } from "./utils/unrealBridge";
import "./globals.css";

const inter = Inter({
  variable: "--font-inter",
  subsets: ["latin"],
});

const bodoniModa = Bodoni_Moda({
  variable: "--font-bodoni-moda",
  subsets: ["latin"],
  weight: ["400", "700"],
});

export default function RootLayout({
  children,
  params,
}: Readonly<{
  children: React.ReactNode;
  params?: Promise<any>;
}>) {
  useEffect(() => {
    // Initialize Unreal Bridge when the app loads
    console.log('Initializing Unreal Bridge...');
    
    // Check if running in Unreal Engine web browser
    const isInUnreal = unrealBridge.isUnrealReady();
    if (isInUnreal) {
      console.log('Running inside Unreal Engine web browser');
      unrealBridge.logToUnreal('NextJS app loaded successfully');
    } else {
      console.log('Running in standalone browser (not in Unreal Engine)');
    }
    
    // Set up screenshot handler
    unrealBridge.onScreenshotFromUnreal((filePath) => {
      console.log('Screenshot received from Unreal:', filePath);
      // This will be handled by specific components that need screenshot data
    });
    
    return () => {
      // Cleanup if needed
    };
  }, []);

  return (
    <html lang="en">
      <body className={`${inter.variable} ${bodoniModa.variable}`} suppressHydrationWarning>
        <ToastProvider position="top-right" maxToasts={5}>
          <ModalProvider>
            {children}
          </ModalProvider>
        </ToastProvider>
      </body>
    </html>
  );
}
