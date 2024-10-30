
#include "TestTriangle.h"
#include "ShaderUtil.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

void TestTriangle::Initialize() {
	m_turningAngle = 0.0f;

	const float positions[] = { -1.0f, -1.0f, 0.0f,
								1.0f, -1.0f, 0.0,
								0.0f, 1.0f, 0.0f };

	m_shaderProgram = ShaderUtil::CreateShaderProgram("VShader.glsl", "FShader.glsl");
	m_transformLocation = glGetUniformLocation(m_shaderProgram, "transformation");

	glGenVertexArrays(1, &m_arrayBufferObject);
	glGenBuffers(1, &m_vertexBufferObject);

	glBindVertexArray(m_arrayBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TestTriangle::Render(float aspectRatio) {
	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_arrayBufferObject);

	glm::mat4 globalTransformation = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f) * 
		glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), m_turningAngle, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(m_transformLocation, 1, GL_FALSE, glm::value_ptr(globalTransformation));
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void TestTriangle::ClearResources() {
	glDeleteBuffers(1, &m_vertexBufferObject);
	glDeleteVertexArrays(1, &m_arrayBufferObject);
	glDeleteProgram(m_shaderProgram);
}

void TestTriangle::Update(double deltaTime) {
	m_turningAngle += glm::radians(120.0f) * (static_cast<float>(deltaTime));
}
