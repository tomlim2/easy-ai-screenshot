'use client';

import { useRouter } from "next/navigation";
import { useMemo } from "react";
import ChatInput from "../components/chat/ChatInput";
import { useSessionContext } from "./layout";
import ConversationHistory from "../components/conversation/ConversationHistory";
import { createApiService } from "../services";
import UnrealTestPanel from "../components/UnrealTestPanel";
import styles from "../components/SessionManagerPanel.module.css";

export default function AppHome() {
  const router = useRouter();
  const apiService = useMemo(() => createApiService(), []);
  const {
    sessionsLoaded,
    handleCreateSession,
    error,
    setError
  } = useSessionContext();

  const handleChatSubmit = async (prompt: string, llmModel?: string) => {
    if (!prompt.trim()) return;
    
    try {
      // Create new session with the first input as the session name
      const newSession = await handleCreateSession(prompt.trim());
      if (newSession) {
        // Send the prompt as the first message to the newly created session
        await apiService.chat(prompt.trim(), newSession.session_id, llmModel);
        
        // Redirect to the newly created session
        router.push(`/app/${newSession.session_id}`);
      }
    } catch (error) {
      console.error('Failed to create session and send first message:', error);
    }
  };

  const refreshContext = () => {
    // No context to refresh on /app page
    console.log('No context to refresh on /app page');
  };

  return (
    <>
      {error && (
        <div className={styles.error}>
          {error}
          <button onClick={() => setError(null)}>×</button>
        </div>
      )}
      
      {/* Add Unreal Test Panel at the top for debugging */}
      <div className="p-4">
        <UnrealTestPanel className="mb-6" />
      </div>
      
      <ConversationHistory 
        context={null}
        loading={false}
        error={null}
        sessionsLoaded={sessionsLoaded}
        isNewSessionPage={true}
      />
      <ChatInput 
        loading={false}
        error={null}
        sessionId={null}
        llmFromDb="gemini-2"
        onSubmit={handleChatSubmit}
        onRefreshContext={refreshContext}
      />
    </>
  );
}