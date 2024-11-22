#pragma once
#include "IGameInterface.h"
#include "CubieRenderer.h"
#include "InputSystem.h"
#include "RubiksCube.h"

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

		float MAX_CAMERA_DISTANCE = 40.0f;
		float MIN_CAMERA_DISTANCE = 10.0f;
		float m_CameraDistance;
		mutable bool m_recalculationNeeded;
		float m_currentAspectRatio;

		//MATRICES
		glm::mat4 m_projection;
		glm::mat4 m_view;
};

