import React from "react";
import { BrowserRouter as Router, Routes, Route, Link, useLocation } from "react-router-dom";
import "./App.css";
import TextChat from "./components/TextChat";
import Register from './components/Register';
import Login from './components/Login';

function App() {
  const Navigation = () => {
    const location = useLocation();

    // 첫 화면("/")에서만 "Login"과 "Register" 버튼 표시
    if (location.pathname === "/") {
      return (
        <div className="top-nav">
          <Link to="/register">
            <button className="nav-button">Register</button>
          </Link>
          <Link to="/login">
            <button className="nav-button">Login</button>
          </Link>
        </div>
      );
    }

    // 다른 화면에서는 아무것도 렌더링하지 않음
    return null;
  };

  return (
    <Router>
      <div className="app-container">
        {/* 네비게이션 */}
        <Navigation />

        {/* 라우트 설정 */}
        <Routes>
          {/* 첫 화면 */}
          <Route
            path="/"
            element={
              <div className="content-box">
                <h1 className="title">English Conversation Start</h1>
                <Link to="/text-chat">
                  <button className="option-button">Text Chat</button>
                </Link>
              </div>
            }
          />
          {/* Text Chat 화면 */}
          <Route path="/text-chat" element={<TextChat />} />
          {/* 회원가입 화면 */}
          <Route path="/register" element={<Register />} />
          {/* 로그인 화면 */}
          <Route path="/login" element={<Login />} />
        </Routes>
      </div>
    </Router>
  );
}

export default App;
