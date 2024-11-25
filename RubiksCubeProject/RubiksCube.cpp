#include "RubiksCube.h"
#include "RubiksGameInterface.h"
#include <glm/gtx/intersect.hpp>
#include <iomanip>
#define NOMINMAX
#include <windows.h>

void RubiksCube::Initialize(const RubiksGameInterface& gameInterface) {
	m_cubieRenderer.Initialize();
	
	float offset = m_cubieRenderer.GetCubieExtension();

	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++) {
				m_grid[x][y][z] = new Cubie;
				m_grid[x][y][z]->m_id = (x * 3 * 3) + (y * 3) + z;
				m_grid[x][y][z]->m_position = glm::vec3((x - 1) * offset, (y - 1) * offset, (z - 1) * offset);
			}
	m_input = &gameInterface.GetInputComponent();
}

void RubiksCube::Render(const glm::mat4& viewProjection) {
	
	glm::mat4 cubeRotation = glm::mat4_cast(m_modelRotation);
	
	for (int x = 0; x < 3; ++x) {
		for (int y = 0; y < 3; ++y) {
			for (int z = 0; z < 3; ++z) {
				glm::mat4 cubieTranslation = glm::translate(glm::mat4(1.0f), m_grid[x][y][z]->m_position);
				glm::mat4 cubieRotation = m_grid[x][y][z]->m_visibleRotation;
				m_cubieRenderer.Render(viewProjection, cubeRotation * cubieRotation * cubieTranslation);
			}
		}
	}
}

void RubiksCube::Update(const RubiksGameInterface& gameInterface)
{
	UpdateMouse();
	m_previousScreenPosition = m_input->GetScreenPosition();
}

void RubiksCube::ClearResources()
{
	m_cubieRenderer.ClearResources();
}

//INPUT
void RubiksCube::UpdateMouse() {

	//LEFT CLICK
	if (m_input->GetActiveMouseButton() == InputSystem::LEFT_BUTTON) {
		
		switch (m_input->GetLeftClickState()) {

		case InputSystem::CLICK:
			std::cout << "CLICK";
			break;

		case InputSystem::HOLD:
			std::cout << "HOLD";
			break;

		case InputSystem::RELEASE:
			std::cout << "RELEASE";
			break;
		}
	}
	//RIGHT CLICK
	else if (m_input->GetActiveMouseButton() == InputSystem::RIGHT_BUTTON) {
		if (m_input->GetRightClickState() == InputSystem::HOLD) {
			RotateCube();
		}
	}
}

//CUBE
void RubiksCube::RotateCube() {
	//Calculate the difference between the current and previous screen positions
	glm::vec2 delta = m_input->GetScreenPosition() - m_previousScreenPosition;

	//Convert the delta vector to radians
	float angleX = glm::radians(delta.x);
	float angleY = glm::radians(delta.y);

	//Create quaternions for the rotations
	glm::quat rotationX = glm::angleAxis(angleX, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat rotationY = glm::angleAxis(angleY, glm::vec3(1.0f, 0.0f, 0.0f));

	//Apply the rotations to the cubeRotation
	m_modelRotation = rotationX * m_modelRotation;
	m_modelRotation = rotationY * m_modelRotation;
	m_modelRotation = glm::normalize(m_modelRotation);
}
