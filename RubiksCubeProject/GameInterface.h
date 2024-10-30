#pragma once

struct GLFWwindow;

class GameInterface {
public:
	virtual void Initialize() {}
	virtual void Initialize(GLFWwindow* winndow) { Initialize(); }

	virtual void Update(double deltaTime) {}
	virtual void Render(float aspectRation) {}

	virtual void ClearResources() {}
};