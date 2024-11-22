#pragma once
#include "CubieRenderer.h"
#include "InputSystem.h"
#include "Cubie.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <array>
#include <glm/ext.hpp>

class RubiksGameInterface;

class RubiksCube {
public:

	void Initialize(const RubiksGameInterface& gameInterface);
	void Render(const glm::mat4& viewProjection);
	void Update(const RubiksGameInterface& gameInterface);
	void ClearResources();


private:

	//INPUT 
	void UpdateMouse();

	//CUBE
	void RotateCube();

	CubieRenderer m_cubieRenderer;
	const InputSystem* m_input;

	//CUBE
	glm::quat m_modelRotation;
	std::array <std::array <std::array <Cubie*, 3>, 3>, 3> m_grid;

	//CACHE
	glm::vec2 m_previousScreenPosition;

};