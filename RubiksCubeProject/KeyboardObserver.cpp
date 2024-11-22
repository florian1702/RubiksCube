//FERTIG

#include "KeyboardObserver.h"
#include "GLFW/glfw3.h"

KeyboardObserver::KeyboardObserver() : KeyboardObserver(nullptr, -1) {
}

KeyboardObserver::KeyboardObserver(GLFWwindow* window, int key) {
	m_window = window;
	m_key = key;

	m_wasDown = false;
	m_wasPressed = false;
	m_wasReleased = false;
}

void KeyboardObserver::Update() {
	bool isDown = glfwGetKey(m_window, m_key) == GLFW_PRESS;

	m_wasPressed = isDown && !m_wasDown;
	m_wasReleased = !isDown && m_wasDown;
	m_wasDown = isDown;

	// mit einer Callbackfunktion kann man den Key abfragen
}
