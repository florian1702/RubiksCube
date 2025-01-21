#include "InputSystem.h"
#include <GLFW/glfw3.h>

void InputSystem::Initialize(GLFWwindow* window, const glm::mat4& projection, const glm::mat4& view) {
	m_window = window;
	SetViewProjection(projection, view);

	//CALLBACK FOR SCROLLLING
	glfwSetScrollCallback(window, ScrollCallback);

	m_activeMouseButton = NO_BUTTON;
}

void InputSystem::Update() {
	//UPDATE KEYBOARD OBSERVERS
	for (auto i = m_keyMapper.begin(); i != m_keyMapper.end(); i++)
		i->second->Update();

	//UPDATE MOUSE STATES
	UpdateClickState(LEFT_BUTTON, m_leftClickState);
	UpdateClickState(RIGHT_BUTTON, m_rightClickState);

	//UPDATE SCREEN POSITION
	double x, y;
	glfwGetCursorPos(m_window, &x, &y);
	m_screenPosition.x = static_cast<float>(x);
	m_screenPosition.y = static_cast<float>(y);
}

void InputSystem::ObserverKey(int key) {
	m_keyMapper[key] = std::make_unique<KeyboardObserver>(KeyboardObserver(m_window, key));
}

void InputSystem::SetViewProjection(const glm::mat4& projection, const glm::mat4& view)
{
	m_viewProjection = projection * view;
	m_projection = projection;
	m_view = view;
}

// GENERATES A PICKING RAY BASED ON THE CURRENT MOUSE POSITION
void InputSystem::GetPickingRay(glm::vec3& out_origin, glm::vec3& out_direction) const {
	// NORMALIZE SCREEN POSITION [-1, 1]
	glm::vec2 position = NormalizeScreenVector(m_screenPosition);
	position.x = (position.x) * 2.0f - 1.0f;
	position.y = 1.0f - (position.y) * 2.0f;

	// DEFINE NEAR AND FAR POINTS IN CLIP SPACE
	glm::vec4 nearPoint = glm::vec4(position.x, position.y, 0.01f, 1.0f);
	glm::vec4 farPoint = nearPoint;
	farPoint.z = 0.99f;

	// TRANSFORM POINTS FROM CLIP SPACE TO WORLD SPACE
	glm::mat4 inverse = glm::inverse(m_viewProjection);
	nearPoint = inverse * nearPoint;
	farPoint = inverse * farPoint;

	// DIVIDE BY W TO RETURN TO 3D COORDINATES
	nearPoint /= nearPoint.w;
	farPoint /= farPoint.w;

	// SET OUTPUT ORIGIN TO NEAR POINT
	out_origin = nearPoint;

	// CALCULATE AND NORMALIZE THE RAY DIRECTION
	out_direction = farPoint - nearPoint;
	out_direction = glm::normalize(out_direction);
}

// NORMALIZES SCREEN POSITION TO THE RANGE [0, 1] BASED ON WINDOW SIZE
glm::vec2 InputSystem::NormalizeScreenVector(const glm::vec2& screenPosition) const {
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(m_window, &screenWidth, &screenHeight);

	glm::vec2 normPos;

	normPos.x = screenPosition.x / screenWidth;
	normPos.y = screenPosition.y / screenHeight;

	return normPos;
}

// CONVERTS WORLD POSITION TO SCREEN COORDINATES
glm::vec2 InputSystem::WorldToScreen(const glm::vec3& worldPosition) const {
	// CREATE A HOMOGENEOUS 4D VECTOR FROM WORLD POSITION
	glm::vec4 h_WorldPosition = glm::vec4(worldPosition, 1.0f);

	// TRANSFORM FROM WORLD TO CLIP SPACE USING VIEW AND PROJECTION MATRICES
	glm::vec4 clipSpacePosition = m_viewProjection * h_WorldPosition;

	// CONVERT FROM CLIP SPACE TO SCREEN SPACE
	glm::vec2 screenPosition;
	screenPosition.x = (clipSpacePosition.x / clipSpacePosition.w + 1.0f) / 2.0f;
	screenPosition.y = (1.0f - (clipSpacePosition.y / clipSpacePosition.w + 1.0f) / 2.0f);

	return screenPosition;
}

// CONVERTS SCREEN POSITION TO WORLD COORDINATES
glm::vec3 InputSystem::ScreenToWorld(const glm::vec2& screenPosition) const {
	// NORMALIZE SCREEN POSITION [-1, 1]
	glm::vec2 position = screenPosition;
	position.x = (position.x) * 2.0f - 1.0f;
	position.y = 1.0f - (position.y) * 2.0f;

	// DEFINE NEAR POINT IN CLIP SPACE
	glm::vec4 nearPoint = glm::vec4(position.x, position.y, -0.99f, 1.0f);

	// TRANSFORM FROM CLIP SPACE TO WORLD SPACE USING INVERSE VIEW PROJECTION MATRIX
	glm::mat4 inverse = glm::inverse(m_viewProjection);
	nearPoint = inverse * nearPoint;

	// DIVIDE BY W TO RETURN TO 3D COORDINATES
	nearPoint /= nearPoint.w;

	return nearPoint;
}


glm::ivec2 InputSystem::GetMouseWheelScrollOffset() const {
	glm::ivec2 mouseScrollOffset = m_mouseScrollOffset;
	m_mouseScrollOffset = glm::ivec2(0, 0);
	return mouseScrollOffset;
}

//HELPING METHODS
void InputSystem::UpdateClickState(MouseButton mouseButton, ClickState& clickState) {

	//BUTTON IS NOT PRESSED
	if (!(glfwGetMouseButton(m_window, mouseButton) == GLFW_PRESS)) {
		//IF PREVIOUSLY CLICKED OR HELD, SET TO RELEASE
		if (clickState == ClickState::CLICK || clickState == ClickState::HOLD)
			clickState = ClickState::RELEASE;
		//OTHERWISE SET TO NO_ACTION IF IT WAS THE ACTIVE BUTTON
		else if (m_activeMouseButton == mouseButton) {
			m_activeMouseButton = NO_BUTTON;
			clickState = ClickState::NO_ACTION;
		}
	}
	//BUTTON WAS RELEASED OR INACTIVE AND NOW PRESSED
	else if (clickState == ClickState::NO_ACTION || clickState == ClickState::RELEASE) {
		m_activeMouseButton = mouseButton;
		clickState = ClickState::CLICK;
		GetPickingRay(m_dragStartRayOrigin, m_dragStartRayDirection);
		m_dragStartScreenPosition = GetScreenPosition();
	}
	//BUTTON IS HELD
	else {
		clickState = ClickState::HOLD;
	}
}

//STATIC
glm::ivec2 InputSystem::m_mouseScrollOffset = glm::vec2(0.0f, 0.0f);