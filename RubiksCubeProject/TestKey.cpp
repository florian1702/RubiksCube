#include "TestKey.h"
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h> 

void TestKey::Initialize(GLFWwindow* window) {
	m_input.SetWindow(window);
	m_input.ObserverKey(GLFW_KEY_SPACE);
	m_input.ObserverKey(GLFW_KEY_RIGHT);
	m_input.ObserverKey(GLFW_KEY_LEFT);
	m_input.ObserverKey(GLFW_KEY_UP);
	m_input.ObserverKey(GLFW_KEY_DOWN);

	m_cubieRenderer.Initialize();
	m_orientationQuaternion = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
}

void TestKey::Render(float aspectRatio) {
	glm::mat4 globalTransformation = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::mat4_cast(m_orientationQuaternion);

	//m_cubieRenderer.Render(globalTransformation);
	m_cubieRenderer.Render(globalTransformation);
}

void TestKey::ClearResources() {
	m_cubieRenderer.ClearResources();
}

void TestKey::Update(double deltaTime) {
	m_input.Update();
	if (m_input.IsKeyDown(GLFW_KEY_SPACE))
		m_orientationQuaternion = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	float xVelocity = 0.0f;
	if (m_input.IsKeyDown(GLFW_KEY_UP))
		xVelocity = glm::radians(90.0f);
	if (m_input.IsKeyDown(GLFW_KEY_DOWN))
		xVelocity = glm::radians(-90.0f);

	float yVelocity = 0.0f;
	if (m_input.IsKeyDown(GLFW_KEY_RIGHT))
		yVelocity = glm::radians(90.0f);
	if (m_input.IsKeyDown(GLFW_KEY_LEFT))
		yVelocity = glm::radians(-90.0f);

	glm::quat velocityQuaternion = glm::quat(0.0f, glm::vec3(xVelocity, yVelocity, 0.0f));

	m_orientationQuaternion += 0.5f * static_cast<float>(deltaTime) * velocityQuaternion * m_orientationQuaternion;
	m_orientationQuaternion = glm::normalize(m_orientationQuaternion);
}
