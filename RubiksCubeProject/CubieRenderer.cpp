#include "CubieRenderer.h"
#include "ShaderUtil.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// INITIALIZES OPENGL BUFFERS, SHADERS, AND TEXTURES FOR RENDERING CUBIES
void CubieRenderer::Initialize() {
	std::vector<glm::vec3> positionField;	// STORES CUBIE VERTEX POSITIONS
	std::vector<glm::vec3> colorField;		// STORES CUBIE VERTEX COLORS
	std::vector<glm::vec2> texCoordField;	// STORES CUBIE TEXTURE COORDS

	// CREATE DATA FOR EACH CUBIE SIDE (X, Y, Z AXES)
	for (int sideType = 0; sideType < 3; sideType++) {
		for (int direction = -1; direction < 2; direction += 2) {
			AddSidePosition(sideType, direction, positionField);
			AddSideColor(sideType, direction, colorField);
			AddSideTexCoords(texCoordField);
		}
	}

	// LOAD SHADER PROGRAM
	m_shaderProgram = ShaderUtil::CreateShaderProgram("VShader.glsl", "FShader.glsl");
	m_transformLocation = glGetUniformLocation(m_shaderProgram, "transformation");

	// SETUP VERTEX ARRAY OBJECT AND BUFFERS
	glGenVertexArrays(1, &m_arrayBufferObject);
	glGenBuffers(3, m_vertexBufferObject);

	glBindVertexArray(m_arrayBufferObject);

	// POSITION BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positionField.size(), positionField.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// COLOR BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colorField.size(), colorField.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// TEXTURE COORDINATES BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoordField.size(), texCoordField.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// LOAD TEXTURE FOR CUBIES
	m_texture = ShaderUtil::LoadTexture("Texture.png");
}

// RENDERS A SINGLE CUBIE USING THE SHADER AND THE TRANSFORMATION MATRICES
void CubieRenderer::Render(const glm::mat4& viewProjection, const glm::mat4& model) {
	// COMPUTE GLOBAL TRANSFORMATION MATRIX
	glm::mat4 globalTransformation = viewProjection * model;

	// ACTIVATE SHADER AND BIND VERTEX ARRAY
	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_arrayBufferObject);

	// SET ACTIVE TEXTURE AND BIND IT
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glUniform1i(glGetUniformLocation(m_shaderProgram, "textureSampler"), 0);

	// SEND TRANSFORMATION MATRIX TO SHADER
	glUniformMatrix4fv(m_transformLocation, 1, GL_FALSE, glm::value_ptr(globalTransformation));

	// DRAW THE CUBIE (36 VERTICES FOR 6 SIDES)
	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);

	// CLEANUP: UNBIND VERTEX ARRAY AND SHADER PROGRAM
	glBindVertexArray(0);
	glUseProgram(0);
}

void CubieRenderer::ClearResources() {
	glDeleteBuffers(3, m_vertexBufferObject);
	glDeleteVertexArrays(1, &m_arrayBufferObject);
	glDeleteProgram(m_shaderProgram);
}

// GENERATES POSITIONS FOR A CUBIE SIDE BASED ON THE AXIS AND DIRECTION
void CubieRenderer::AddSidePosition(int sideType, int direction, std::vector<glm::vec3>& positionArray) {
	glm::vec3 cornerPoints[2][2];

	int localXDim = (sideType + 1) % 3;
	int localYDim = (sideType + 2) % 3;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			glm::vec3 localPoint(direction * OFFSET);
			localPoint[localXDim] = (2 * i - 1) * OFFSET;
			localPoint[localYDim] = (2 * j - 1) * OFFSET;
			cornerPoints[i][j] = localPoint;
		}
	}

	// DEFINE TRIANGLES FOR THE SIDE (2 TRIANGLES = 6 VERTICES)
	positionArray.push_back(cornerPoints[0][0]);
	positionArray.push_back(cornerPoints[1][0]);
	positionArray.push_back(cornerPoints[0][1]);
	positionArray.push_back(cornerPoints[1][0]);
	positionArray.push_back(cornerPoints[0][1]);
	positionArray.push_back(cornerPoints[1][1]);
}

// GENERATES COLORS FOR A CUBIE SIDE BASED ON THE AXIS AND DIRECTION
void CubieRenderer::AddSideColor(int sideType, int direction, std::vector<glm::vec3>& colorArray) {
	glm::vec3 color = glm::vec3(0.0f);

	// ASSIGN COLOR BASED ON AXIS AND DIRECTION
	if (sideType == 0) { color = (direction == 1) ? RED : ORANGE; }
	else if (sideType == 1) { color = (direction == 1) ? GREEN : BLUE; }
	else if (sideType == 2) { color = (direction == 1) ? YELLOW : WHITE; }

	for (int i = 0; i < 6; i++) {
		colorArray.push_back(color);
	}
}

// ADDS TEXTURE COORDINATES FOR A CUBIE SIDE
void CubieRenderer::AddSideTexCoords(std::vector<glm::vec2>& texCoordArray) {
	texCoordArray.push_back(glm::vec2(0.0f, 0.0f));
	texCoordArray.push_back(glm::vec2(1.0f, 0.0f));
	texCoordArray.push_back(glm::vec2(0.0f, 1.0f));
	texCoordArray.push_back(glm::vec2(1.0f, 0.0f));
	texCoordArray.push_back(glm::vec2(0.0f, 1.0f));
	texCoordArray.push_back(glm::vec2(1.0f, 1.0f));
}

