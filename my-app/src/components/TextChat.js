import React, { useState, useEffect } from "react";
import "./TextChat.css"

export default function TextChat() {
  const [messages, setMessages] = useState([]);
  const [inputText, setInputText] = useState("");
  const [socket, setSocket] = useState(null);

  // 고유 사용자 ID 생성
  const userId = React.useMemo(() => Math.random().toString(36).substr(2, 9), []);

  useEffect(() => {
    const ws = new WebSocket("ws://localhost:4000");

    ws.onopen = () => {
      console.log("WebSocket connection established");
      setSocket(ws);
    };

    ws.onclose = () => {
      console.error("WebSocket connection closed. Reconnecting in 3 seconds...");
      setTimeout(() => {
        const newWs = new WebSocket("ws://localhost:4000");
        setSocket(newWs);
      }, 3000);
    };

    ws.onerror = (error) => {
      console.error("WebSocket error:", error.message);
    };

    ws.onmessage = (event) => {
      try {
        const receivedMessage = JSON.parse(event.data);
        console.log("Received message:", receivedMessage);

        // 자신이 보낸 메시지가 아닌 경우에만 추가
        if (receivedMessage.userId !== userId) {
          setMessages((prevMessages) => [
            ...prevMessages,
            { text: receivedMessage.text, isUser: false },
          ]);
        }
      } catch (e) {
        console.error("Failed to parse message:", e);
      }
    };

    return () => {
      ws.close();
    };
  }, [userId]);

  const sendMessage = () => {
    if (inputText.trim() && socket && socket.readyState === WebSocket.OPEN) {
      const message = JSON.stringify({ userId, text: inputText }); // userId 추가
      socket.send(message);
      setMessages((prevMessages) => [
        ...prevMessages,
        { text: inputText, isUser: true },
      ]);
      setInputText("");
    } else {
      console.error("Cannot send message. WebSocket not connected.");
    }
  };

  return (
    <div className="chat-container">
      <h1 className="title">Text Chat</h1>
      <div className="messages">
        {messages.map((message, index) => (
          <div
            key={index}
            className={`message ${message.isUser ? "user" : "other"}`}
          >
            {message.text}
          </div>
        ))}
      </div>
      <div className="input-container">
        <input
          type="text"
          className="input"
          placeholder="Type a message..."
          value={inputText}
          onChange={(e) => setInputText(e.target.value)}
          onKeyPress={(e) => e.key === "Enter" && sendMessage()}
        />
        <button className="send-button" onClick={sendMessage}>
          Send
        </button>
      </div>
    </div>
  );
}
