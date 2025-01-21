#pragma once
#include "IGameInterface.h"
#include "InputSystem.h"
#include "RubiksCube.h"

// RUBIKSGAMEINTERFACE CLASS: MANAGES GAME LOGIC, INPUT, RENDERING, AND CAMERA SETTINGS
class RubiksGameInterface : public IGameInterface
{
	public:
		virtual void Initialize(GLFWwindow* window) override;
		virtual void Render(float aspectRatio)override;
		virtual void ClearResources() override;
		virtual void Update(double deltaTime) override;

		const InputSystem& GetInputComponent() const { return m_input; }
		float GetDeltaTime() const { return m_deltaTime; }

		// TRIGGERS MATRIX RECALCULATION FOR THE NEXT FRAME
		void QueueMatrixRecalculation() const { m_recalculationNeeded = true; }

	private:
		InputSystem m_input;
		RubiksCube m_rubiksCube;
		mutable GLFWwindow* m_window;
		float m_deltaTime;

		// CAMERA SETTINGS
		const float MAX_CAMERA_DISTANCE = 20.0f;
		const float MIN_CAMERA_DISTANCE = 7.5f;
		float m_CameraDistance = 10.0f;

		// MATRIX RECALCULATION FLAG
		mutable bool m_recalculationNeeded;
		float m_currentAspectRatio;

		//MATRICES
		glm::mat4 m_projection;
		glm::mat4 m_view;
};

