#pragma once
#include "GameInterface.h"

#include <glm/ext/quaternion_float.hpp>

class TestGlm : public GameInterface {
public:
	void Initialize() override;
	void Update(double deltaTime) override;
	
private:
	glm::quat m_oreintaionQuaternion;
};