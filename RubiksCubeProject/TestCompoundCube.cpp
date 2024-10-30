#include "TestCompoundCube.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

void TestCompoundCube::Initialize()
{
	m_cubieRenderer.Initialize();
	m_turningAngle = 0.0f;
}

void TestCompoundCube::Render(float aspectRatio)
{
	glm::mat4 globalTransformation = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f) *
		glm::lookAt(glm::vec3(0.0f, 0.0f, -9.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), m_turningAngle, glm::vec3(1.0f, 1.0f, 1.0f));

	//Offset + 0.1f damit die Luecken zwischen den Minicubies erscheinen.
	float offset = m_cubieRenderer.GetCubieExtension() + 0.1f;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				glm::mat4 compound = glm::translate(globalTransformation, glm::vec3((i - 1) * offset, (j - 1) * offset, (k - 1) * offset));

				//Rotation der mittleren horizontalen auf jeder Seite
				//compound = glm::rotate(compound, glm::radians(90.0f) * (i % 2), glm::vec3(1.0f, 0.0f, 0.0f));
				//compound = glm::rotate(compound, glm::radians(90.0f) * (j % 2), glm::vec3(0.0f, 1.0f, 0.0f));
				//compound = glm::rotate(compound, glm::radians(90.0f) * (k % 2), glm::vec3(0.0f, 0.0f, 1.0f));
				m_cubieRenderer.Render(compound);
			}
		}
	}
}

void TestCompoundCube::ClearResources()
{
	m_cubieRenderer.ClearResources();
}

void TestCompoundCube::Update(double deltaTime)
{
	m_turningAngle += glm::radians(120.0f) * ((float)deltaTime);
}