import React, { useState } from "react";
import { useNavigate, Link } from "react-router-dom";
import "./Form.css";

function Login() {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [message, setMessage] = useState("");
  const navigate = useNavigate();

  const handleLogin = async () => {
    try {
      const response = await fetch("http://localhost:8080/login", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ username, password }),
      });

      if (!response.ok) {
        const errorData = await response.json();
        setMessage(errorData.error || "Failed to login.");
        return;
      }

      const data = await response.json();
      setMessage(data.message || "Login successful!");
      navigate("/text-chat");
    } catch (error) {
      console.error("Error:", error);
      setMessage("Failed to connect to the server.");
    }
  };

  return (
    <div className="form-container">
      {/* Back to Home 버튼 */}
      <div className="register-top-nav">
        <button className="back-home-button" onClick={() => navigate("/")}>
          Home
        </button>
      </div>

      <h1>Login</h1>
      <input
        type="text"
        placeholder="Username"
        value={username}
        onChange={(e) => setUsername(e.target.value)}
      />
      <input
        type="password"
        placeholder="Password"
        value={password}
        onChange={(e) => setPassword(e.target.value)}
      />
      <button onClick={handleLogin}>Login</button>
      {message && <p className="form-message">{message}</p>}

      {/* 회원가입 버튼 */}
      <p>Don't have an account?</p>
      <Link to="/register">
        <button className="secondary-button">Go to Register</button>
      </Link>
    </div>
  );
}

export default Login;
