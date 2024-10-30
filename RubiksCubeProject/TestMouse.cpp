#include "TestMouse.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

void TestMouse::Initialize(GLFWwindow* window)
{
	m_input.SetWindow(window);
	m_cubePosition = glm::vec3(0.0f);
	m_viewProject = glm::mat4(1.0f);
	m_cubieRenderer.Initialize();
}

void TestMouse::Render(float aspectRatio)
{
	m_viewProject = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, -9.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 finalMat = glm::translate(m_viewProject, m_cubePosition);

	m_cubieRenderer.Render(finalMat);
}

void TestMouse::ClearResources()
{
	m_cubieRenderer.ClearResources();
}

void TestMouse::Update(double deltaTime)
{
	if (!m_input.IsLeftMouseButtonDown())
	{
		return;
	}

	glm::vec3 position, direction;

	m_input.GetPickingRay(m_viewProject, position, direction);
	m_cubePosition = position + 6.0f * direction;
}