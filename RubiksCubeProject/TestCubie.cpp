#include "TestCubie.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

void TestCubie::Initialize() {
	m_cubieRenderer.Initialize();
	m_turningAngle = 0.0f;
}

void TestCubie::Render(float aspectRatio) {
	glm::mat4 globalTransformation = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), m_turningAngle, glm::vec3(1.0f, 1.0f, 1.0f));

	//m_cubieRenderer.Render(globalTransformation);
	m_cubieRenderer.Render(aspectRatio);

}

void TestCubie::ClearResources() {
	m_cubieRenderer.ClearResources();
}

void TestCubie::Update(double deltaTime) {
	m_turningAngle += glm::radians(120.0f) * (static_cast<float>(deltaTime));
}
