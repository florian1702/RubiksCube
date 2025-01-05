#include "CubieRenderer.h"
#include "ShaderUtil.h"
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <iostream>

void CubieRenderer::Initialize() {
	std::vector<glm::vec3> positionField;
	std::vector<glm::vec3> colorField;
	std::vector<glm::vec2> texCoordField;

	// Build the cube information
	for (int sideType = 0; sideType < 3; sideType++) {
		for (int direction = -1; direction < 2; direction += 2) {
			AddSidePosition(sideType, direction, positionField);
			AddSideColor(sideType, direction, colorField);
			AddSideTexCoords(texCoordField);
		}
	}

	m_shaderProgram = ShaderUtil::CreateShaderProgram("VShader.glsl", "FShader.glsl");
	m_transformLocation = glGetUniformLocation(m_shaderProgram, "transformation");

	glGenVertexArrays(1, &m_arrayBufferObject);
	glGenBuffers(3, m_vertexBufferObject);

	glBindVertexArray(m_arrayBufferObject);

	// POSITION
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positionField.size(), positionField.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// COLOR
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colorField.size(), colorField.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// TEXTURE COORDINATES
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoordField.size(), texCoordField.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Load Texture
	m_texture = ShaderUtil::LoadTexture("Texture.png");
}


void CubieRenderer::Render(const glm::mat4& viewProjection, const glm::mat4& model) {
	glm::mat4 globalTransformation = viewProjection * model;
	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_arrayBufferObject);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glUniform1i(glGetUniformLocation(m_shaderProgram, "textureSampler"), 0);

	glUniformMatrix4fv(m_transformLocation, 1, GL_FALSE, glm::value_ptr(globalTransformation));
	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);

	glBindVertexArray(0);
	glUseProgram(0);
}

void CubieRenderer::ClearResources() {
	glDeleteBuffers(2, m_vertexBufferObject);
	glDeleteVertexArrays(1, &m_arrayBufferObject);
	glDeleteProgram(m_shaderProgram);
}

float CubieRenderer::GetCubieExtension() {
	return 2.0f * m_offset;
}

void CubieRenderer::AddSidePosition(int sideType, int direction, std::vector<glm::vec3>& positionArray) {
	glm::vec3 cornerPoints[2][2];

	int localXDim = (sideType + 1) % 3;
	int localYDim = (sideType + 2) % 3;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			glm::vec3 localPoint(direction * m_offset);
			localPoint[localXDim] = (2 * i - 1) * m_offset;
			localPoint[localYDim] = (2 * j - 1) * m_offset;
			cornerPoints[i][j] = localPoint;
		}
	}
	positionArray.push_back(cornerPoints[0][0]);
	positionArray.push_back(cornerPoints[1][0]);
	positionArray.push_back(cornerPoints[0][1]);
	positionArray.push_back(cornerPoints[1][0]);
	positionArray.push_back(cornerPoints[0][1]);
	positionArray.push_back(cornerPoints[1][1]);
}

void CubieRenderer::AddSideColor(int sideType, int direction, std::vector<glm::vec3>& colorArray) {
	glm::vec3 color = glm::vec3(0.0f);

	// Assign colors based on sideType and direction
	if (sideType == 0) { // X-axis sides
		color = (direction == 1) ? RED : ORANGE;
	}
	else if (sideType == 1) { // Y-axis sides
		color = (direction == 1) ? GREEN : BLUE;
	}
	else if (sideType == 2) { // Z-axis sides
		color = (direction == 1) ? YELLOW : WHITE;
	}

	// Add the color to the array 6 times (one for each vertex of the two triangles forming the side)
	for (int i = 0; i < 6; i++) {
		colorArray.push_back(color);
	}
}

void CubieRenderer::AddSideTexCoords(std::vector<glm::vec2>& texCoordArray) {
	texCoordArray.push_back(glm::vec2(0.0f, 0.0f));
	texCoordArray.push_back(glm::vec2(1.0f, 0.0f));
	texCoordArray.push_back(glm::vec2(0.0f, 1.0f));
	texCoordArray.push_back(glm::vec2(1.0f, 0.0f));
	texCoordArray.push_back(glm::vec2(0.0f, 1.0f));
	texCoordArray.push_back(glm::vec2(1.0f, 1.0f));
}

