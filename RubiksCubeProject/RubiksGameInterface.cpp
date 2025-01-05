#include "RubiksGameInterface.h"
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>

void RubiksGameInterface::Initialize(GLFWwindow* window)
{
	m_window = window;

	//SETUP INPUT SYSTEM
	m_input.Initialize(window);
	m_input.ObserverKey(GLFW_KEY_SPACE);
	m_input.ObserverKey(GLFW_KEY_RIGHT);
	m_input.ObserverKey(GLFW_KEY_LEFT);
	m_input.ObserverKey(GLFW_KEY_UP);
	m_input.ObserverKey(GLFW_KEY_DOWN);

	//SETUP RUBIKS CUBE
	m_rubiksCube.Initialize(*this);
}

void RubiksGameInterface::Render(float aspectRatio)
{
	// RECALCULATE MATRICES IF NEEDED
	if (m_recalculationNeeded || aspectRatio != m_currentAspectRatio) {
		m_currentAspectRatio = aspectRatio;

		m_projection = glm::perspective(glm::radians(45.0f), m_currentAspectRatio, 0.1f, 100.0f);
		m_view = glm::lookAt(glm::vec3(0.0f, 0.0f, m_CameraDistance), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		m_input.SetViewProjection(m_projection, m_view);
		m_recalculationNeeded = false;
	}

	//RENDER RUBIKS CUBE
	m_rubiksCube.Render(m_projection * m_view);
}

void RubiksGameInterface::Update(double deltaTime)
{
	m_deltaTime = static_cast<float>(deltaTime);

	//UPDATE INPUT SYSTEM
	m_input.Update();

	//UPDATE OR RESET RUBIKS CUBE
	if (m_input.WasKeyReleased(GLFW_KEY_SPACE)) {
		m_rubiksCube.ClearResources();
		//m_rubiksCube = RubiksCube();
		m_rubiksCube.Initialize(*this);
	}
	else
		m_rubiksCube.Update(*this);

	// UPDATE CAMERA DISTANCE
	m_CameraDistance -= m_input.GetMouseWheelScrollOffset().y;
	if (m_CameraDistance < MIN_CAMERA_DISTANCE)
		m_CameraDistance = MIN_CAMERA_DISTANCE;
	if (m_CameraDistance > MAX_CAMERA_DISTANCE)
		m_CameraDistance = MAX_CAMERA_DISTANCE;

	QueueMatrixRecalculation();

}

void RubiksGameInterface::ClearResources()
{
	m_rubiksCube.ClearResources();
}