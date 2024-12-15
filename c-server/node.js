const WebSocket = require("ws");
const net = require("net");

const WS_PORT = 4000;
const TCP_PORT = 8000;

// WebSocket 서버 생성
const wss = new WebSocket.Server({ port: WS_PORT });

wss.on("connection", (ws) => {
  console.log("New WebSocket client connected");

  // TCP 클라이언트 생성
  const tcpClient = new net.Socket();

  tcpClient.connect(TCP_PORT, "localhost", () => {
    console.log("Connected to TCP server");
  });

  tcpClient.on("data", (data) => {
    console.log("Message from TCP server:", data.toString());
    ws.send(data.toString()); // WebSocket으로 전달
  });

  tcpClient.on("close", () => {
    console.log("TCP connection closed");
  });

  tcpClient.on("error", (err) => {
    console.error("TCP error:", err.message);
  });

  ws.on("message", (message) => {
    console.log("Message from WebSocket client:", message);
    tcpClient.write(message); // TCP로 메시지 전송
  });

  ws.on("close", () => {
    console.log("WebSocket client disconnected");
    tcpClient.destroy(); // TCP 소켓 종료
  });

  ws.on("error", (err) => {
    console.error("WebSocket error:", err.message);
  });
});
