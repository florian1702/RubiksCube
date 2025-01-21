#include "RubiksGameInterface.h"
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>

void RubiksGameInterface::Initialize(GLFWwindow* window)
{
	m_window = window;

	// SETUP INPUT SYSTEM AND ADD SPACE KEY OBSERVER
	m_input.Initialize(window);
	m_input.ObserverKey(GLFW_KEY_SPACE);

	// SETUP RUBIKS CUBE
	m_rubiksCube.Initialize(*this);
}

// RENDERS THE RUBIKS CUBE
void RubiksGameInterface::Render(float aspectRatio)
{
	// RECALCULATE MATRICES IF CAMERA OR ASPECT RATIO HAS CHANGED
	if (m_recalculationNeeded || aspectRatio != m_currentAspectRatio) {
		m_currentAspectRatio = aspectRatio;

		// CREATE PERSPECTIVE PROJECTION MATRIX
		m_projection = glm::perspective(glm::radians(45.0f), m_currentAspectRatio, 0.1f, 100.0f);

		// CREATE CAMERA VIEW MATRIX
		m_view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, m_CameraDistance),  // CAMERA POSITION
			glm::vec3(0.0f, 0.0f, 0.0f),              // CAMERA TARGET
			glm::vec3(0.0f, 1.0f, 0.0f)               // UP VECTOR
		);

		// UPDATE INPUT SYSTEM WITH NEW MATRICES
		m_input.SetViewProjection(m_projection, m_view);
		m_recalculationNeeded = false;
	}

	// RENDER RUBIKS CUBE
	m_rubiksCube.Render(m_projection * m_view);
}

// UPDATES GAME LOGIC
void RubiksGameInterface::Update(double deltaTime)
{
	m_deltaTime = static_cast<float>(deltaTime);

	// UPDATE INPUT SYSTEM
	m_input.Update();

	// CHECK IF SPACE KEY WAS RELEASED TO RESET THE RUBIK'S CUBE
	if (m_input.WasKeyReleased(GLFW_KEY_SPACE)) {
		m_rubiksCube.ClearResources();
		m_rubiksCube.Initialize(*this);
	}
	else
		// UPDATE THE RUBIK'S CUBE STATE
		m_rubiksCube.Update(*this);

	// ADJUST CAMERA DISTANCE BASED ON MOUSE SCROLL
	m_CameraDistance -= m_input.GetMouseWheelScrollOffset().y;

	// LIMIT CAMERA DISTANCE WITHIN DEFINED BOUNDS
	if (m_CameraDistance < MIN_CAMERA_DISTANCE)
		m_CameraDistance = MIN_CAMERA_DISTANCE;
	if (m_CameraDistance > MAX_CAMERA_DISTANCE)
		m_CameraDistance = MAX_CAMERA_DISTANCE;

	// MATRIX RECALCULATION FOR NEXT FRAME
	QueueMatrixRecalculation();
}

void RubiksGameInterface::ClearResources()
{
	m_rubiksCube.ClearResources();
}