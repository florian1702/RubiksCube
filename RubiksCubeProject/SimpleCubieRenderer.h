#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include <vector>

class SimpleCubieRenderer {
public:
	void Initialize();
	void Render(const glm::mat4& transformationMatrix);
	void ClearResources();

	float GetCubieExtension();

private:
	const float m_offset = 0.5f;

	void AddSidePosition(int sideType, int direction, std::vector<glm::vec3>& positionArray);
	void AddSideColor(int sideType, int direction, std::vector<glm::vec3>& colorArray);

	GLuint m_arrayBufferObject;
	GLuint m_vertexBufferObject[2];
	GLuint m_shaderProgram;
	GLuint m_transformLocation;
};