//FERTIG

#pragma once

struct GLFWwindow;

class IGameInterface {
public:
	virtual void Initialize() {}
	virtual void Initialize(GLFWwindow* winndow) { Initialize(); }

	virtual void Update(double deltaTime) {}
	virtual void Render(float aspectRation) {}

	virtual void ClearResources() {}
};