#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include <vector>

// CUBIERENDERER CLASS: RENDERS INDIVIDUAL CUBIES FOR THE RUBIK'S CUBE
class CubieRenderer {
public:
	void Initialize();
	void Render(const glm::mat4& viewProjection, const glm::mat4& model);
	void ClearResources();
	// RETURNS THE SIZE OF A CUBIE
	float GetCubieExtension() { return 2.0f * OFFSET; }
	
private:
	// OFFSET FOR CUBIE POSITIONING
	const float OFFSET = 0.5f; 

	// SIDE COLORS
	const glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 GREEN = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 BLUE = glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::vec3 ORANGE = glm::vec3(1.0f, 0.5f, 0.0f);
	const glm::vec3 YELLOW = glm::vec3(1.0f, 1.0f, 0.0f);
	const glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);

	// HELPER FUNCTIONS FOR SIDE DATA
	void AddSidePosition(int sideType, int direction, std::vector<glm::vec3>& positionArray);
	void AddSideColor(int sideType, int direction, std::vector<glm::vec3>& colorArray);
	void AddSideTexCoords(std::vector<glm::vec2>& texCoordArray);

	// OPENGL BUFFERS AND RESOURCES
	GLuint m_arrayBufferObject; // VERTEX ARRAY OBJECT
	GLuint m_vertexBufferObject[3]; // POSITION, COLOR, TEXTURE COORD BUFFERS
	GLuint m_shaderProgram; // SHADER PROGRAM ID
	GLuint m_transformLocation; // UNIFORM LOCATION FOR TRANSFORMATION MATRIX
	GLuint m_texture; // TEXTURE ID
};