#include "InputSystem.h"
#include <GLFW/glfw3.h>

void InputSystem::Update() {
	for (auto i = m_keyMapper.begin(); i != m_keyMapper.end(); i++)
		i->second->Update();
}

void InputSystem::ObserverKey(int key) {
	m_keyMapper[key] = std::make_unique<KeyboardObserver>(KeyboardObserver(m_window, key));
}

bool InputSystem::IsLeftMouseButtonDown() {
	return glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}

bool InputSystem::IsRightMouseButtonDown() {
	return glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

void InputSystem::GetPickingRay(const glm::mat4& transfomationMatrix, glm::vec3& startingPoint, glm::vec3& direction) {
	double xPosition, yPosition;
	GetMousePosition(xPosition, yPosition);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(m_window, &screenWidth, &screenHeight);

	xPosition = (xPosition / screenWidth) * 2.0f - 1.0f;
	yPosition = 1.0f - (yPosition / screenHeight) * 2.0f;

	glm::vec4 nearPoint = glm::vec4(static_cast<float>(xPosition), static_cast<float>(yPosition), -0.99f, 1.0f);
	glm::vec4 farPoint = nearPoint;
	farPoint.z = 0.99f;

	glm::mat4 inverse = glm::inverse(transfomationMatrix);
	nearPoint = inverse * nearPoint;
	farPoint = inverse * farPoint;

	nearPoint /= nearPoint.w;
	farPoint /= farPoint.w;

	startingPoint = nearPoint;
	direction = farPoint - nearPoint;

	direction = glm::normalize(direction);
}

void InputSystem::GetMousePosition(double& xPosition, double& yPosition) {
	glfwGetCursorPos(m_window, &xPosition, &yPosition);
}
