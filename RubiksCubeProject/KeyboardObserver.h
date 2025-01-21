#pragma once

struct GLFWwindow;

// KEYBOARDOBSERVER CLASS: OBSERVES THE STATE OF A SPECIFIC KEY
class KeyboardObserver {
public:
	KeyboardObserver();
	KeyboardObserver(GLFWwindow* window, int key);
	void Update();

	bool m_wasDown;
	bool m_wasPressed;
	bool m_wasReleased;

private:
	GLFWwindow* m_window;
	int m_key;
};

