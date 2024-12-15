import { render, screen } from "@testing-library/react";
import App from "./App";

test("renders Choose Conversation Type", () => {
  render(<App />);
  const titleElement = screen.getByText(/Choose Conversation Type/i);
  expect(titleElement).toBeInTheDocument();
});

test("renders Text Chat button", () => {
  render(<App />);
  const textChatButton = screen.getByText(/Text Chat/i);
  expect(textChatButton).toBeInTheDocument();
});
