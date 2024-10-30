#pragma once
#include "GameInterface.h"
#include "CubieRenderer.h"

class TestCompoundCube : public GameInterface
{
public:
	virtual void Initialize();
	virtual void Render(float aspectRatio);
	virtual void ClearResources();
	virtual void Update(double deltaTime);

private:
	CubieRenderer m_cubieRenderer;
	float m_turningAngle;
};