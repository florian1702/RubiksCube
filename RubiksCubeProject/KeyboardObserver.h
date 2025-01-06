#pragma once

struct GLFWwindow;

// Class to observe the state of a specific key in a GLFW window
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

