import React, { useState } from "react";
import { useNavigate, Link } from "react-router-dom";
import "./Form.css";

function Register() {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [message, setMessage] = useState("");
  const navigate = useNavigate();

  const handleRegister = async () => {
    try {
      const response = await fetch("http://localhost:8080/register", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ username, password }),
      });

      if (!response.ok) {
        const errorData = await response.json();
        setMessage(errorData.error || "Failed to register.");
        return;
      }

      const data = await response.json();
      setMessage(data.message || "Registration successful!");
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

      <h1>Register</h1>
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
      <button onClick={handleRegister}>Register</button>
      {message && <p className="form-message">{message}</p>}

      {/* 로그인 버튼 */}
      <p>Already have an account?</p>
      <Link to="/login">
        <button className="secondary-button">Go to Login</button>
      </Link>
    </div>
  );
}

export default Register;
