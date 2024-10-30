
#include "TestGlm.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"


void TestGlm::Initialize() {
	// Translation
	glm::mat4 sitzgruppe = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
	glm::mat4 lokalstuhl = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 1.0f));

	//Rotation
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//Skalierung
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));

	//Kameratransformation
	glm::mat4 look = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//Perspektive
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);

	//Skalarprodukt
	float testA = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // = 0.0 (beide Vektoren liegen seknrecht aufeinander)
	
	//Kreuzprodukt
	glm::vec3 cross = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 stuhl = sitzgruppe * rotate * lokalstuhl;
	glm::mat4 stuhlRender = perspective * look * stuhl;

	//Quaternions
	m_oreintaionQuaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

void TestGlm::Update(double deltaTime) {
	//Quaternions (Übergang)
	glm::quat rotationVelocity = glm::quat(0.0f, glm::radians(180.0f), 0.0f, 0.0f);
	glm::quat timeDerivatave = 0.5f * rotationVelocity * m_oreintaionQuaternion;
	m_oreintaionQuaternion += (static_cast<float>(deltaTime) * timeDerivatave);
	m_oreintaionQuaternion = glm::normalize(m_oreintaionQuaternion);
	glm::mat4 rotationMatrix = glm::mat4_cast(m_oreintaionQuaternion);
}
