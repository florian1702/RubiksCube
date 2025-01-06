#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include <vector>

// The CubieRenderer class is responsible for rendering Rubik's Cube cubies, including their positions, colors, and textures.
class CubieRenderer {
public:
	void Initialize();
	void Render(const glm::mat4& viewProjection, const glm::mat4& model);
	void ClearResources();

	// Returns the size of the cubie
	float GetCubieExtension();
	
private:
	const float m_offset = 0.5f; // Offset for cubie positioning

	// Cubie colors
	const glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 GREEN = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 BLUE = glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::vec3 ORANGE = glm::vec3(1.0f, 0.5f, 0.0f);
	const glm::vec3 YELLOW = glm::vec3(1.0f, 1.0f, 0.0f);
	const glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);


	void AddSidePosition(int sideType, int direction, std::vector<glm::vec3>& positionArray);
	void AddSideColor(int sideType, int direction, std::vector<glm::vec3>& colorArray);
	void AddSideTexCoords(std::vector<glm::vec2>& texCoordArray);

	GLuint m_arrayBufferObject;
	GLuint m_vertexBufferObject[3];
	GLuint m_shaderProgram;
	GLuint m_transformLocation;
	GLuint m_texture;
};