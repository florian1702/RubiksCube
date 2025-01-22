#pragma once
#include <glm/glm.hpp>

// REPRESENTS A SINGLE CUBIE IN THE RUBIKS CUBE
struct Cubie {
	int m_id;
	glm::vec3 m_position = glm::vec3(0.0f);
	glm::mat4 m_visibleRotation = glm::mat4(1.0f);
	glm::mat4 m_snapedRotation = glm::mat4(1.0f);
};