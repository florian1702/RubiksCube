#pragma once
#include "IGameInterface.h"
#include "InputSystem.h"
#include "RubiksCube.h"

// The RubiksGameInterface class manages the Rubik's Cube game logic, including user input, rendering, matrix calculations, and updates.
class RubiksGameInterface : public IGameInterface
{
	public:
		virtual void Initialize(GLFWwindow* window) override;
		virtual void Render(float aspectRatio)override;
		virtual void ClearResources() override;
		virtual void Update(double deltaTime) override;

		const InputSystem& GetInputComponent() const { return m_input; }
		const glm::mat4& GetProjectionMatrix() const { return m_projection; }
		const glm::mat4& GetViewMatrix() const { return m_view; }
		GLFWwindow* GetWindow() const { return m_window; }
		float GetDeltaTime() const { return m_deltaTime; }

		void QueueMatrixRecalculation() const { m_recalculationNeeded = true; }

	private:
		InputSystem m_input;
		RubiksCube m_rubiksCube;
		mutable GLFWwindow* m_window;
		float m_deltaTime;

		// Constants for camera zoom level limits
		float MAX_CAMERA_DISTANCE = 20.0f;
		float MIN_CAMERA_DISTANCE = 7.5f;
		float m_CameraDistance = 10.0f;

		// Matrix recalculation flag
		mutable bool m_recalculationNeeded;
		float m_currentAspectRatio;

		//MATRICES
		glm::mat4 m_projection;
		glm::mat4 m_view;
};

