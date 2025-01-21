#pragma once
#include "KeyboardObserver.h"
#include <map>
#include <memory>
#include <glm/mat4x4.hpp>

// The InputSystem class handles keyboard and mouse input, including key states, mouse actions, and 3D-to-screen transformations.
class InputSystem {
public:

	enum ClickState {
		NO_ACTION = 0,
		CLICK = 1,
		HOLD = 2,
		RELEASE = 3
	};

	enum MouseButton {
		NO_BUTTON = -1,
		LEFT_BUTTON = 0,
		RIGHT_BUTTON = 1
	};

	//GENERAL
	void Initialize(GLFWwindow* window, const glm::mat4& projection = glm::mat4(1.0f), const glm::mat4& view = glm::mat4(1.0f));
	void Update();
	void ObserverKey(int key);
	void SetViewProjection(const glm::mat4& projection, const glm::mat4& view);

	//MOUSE
	InputSystem::ClickState GetLeftClickState() const { return m_leftClickState; }
	InputSystem::ClickState GetRightClickState() const { return m_rightClickState; }
	glm::vec2 GetScreenPosition() const { return m_screenPosition; }
	glm::vec2 GetDragStartScreenPosition() const { return m_dragStartScreenPosition; }
	void GetPickingRay(glm::vec3& out_origin, glm::vec3& out_direction) const;
	glm::ivec2 GetMouseWheelScrollOffset() const;

	//KEYBOARD
	bool WasKeyDown(int key) { return m_keyMapper[key]->m_wasDown; };
	bool WasKeyPressed(int key) { return m_keyMapper[key]->m_wasPressed; };
	bool WasKeyReleased(int key) { return m_keyMapper[key]->m_wasReleased; };
	MouseButton GetActiveMouseButton() const { return m_activeMouseButton; }

	//HELPING METHODS
	glm::vec2 NormalizeScreenVector(const glm::vec2& screenPosition) const;
	glm::vec2 WorldToScreen(const glm::vec3& worldPosition) const;
	glm::vec3 ScreenToWorld(const glm::vec2& screenPosition) const;

private:
	std::map<int, std::unique_ptr<KeyboardObserver>> m_keyMapper;
	GLFWwindow* m_window;

	MouseButton m_activeMouseButton;
	ClickState m_leftClickState;
	ClickState m_rightClickState;

	glm::vec2 m_screenPosition;
	glm::vec2 m_dragStartScreenPosition;
	glm::vec3 m_dragStartRayOrigin;
	glm::vec3 m_dragStartRayDirection;

	glm::mat4 m_viewProjection;
	glm::mat4 m_view;
	glm::mat4 m_projection;


	void UpdateClickState(MouseButton mouseButton, ClickState& clickState);

	//STATIC
	static glm::ivec2 m_mouseScrollOffset;
	static void ScrollCallback(GLFWwindow* window, double xScroll, double yScroll) {
		m_mouseScrollOffset.x = static_cast<int>(xScroll);
		m_mouseScrollOffset.y = static_cast<int>(yScroll);
	}

};