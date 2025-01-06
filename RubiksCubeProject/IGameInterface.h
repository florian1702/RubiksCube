#pragma once

struct GLFWwindow;

// Interface class for game-specific logic (to be extended by concrete game interfaces)
class IGameInterface {
public:
	virtual void Initialize() {}
	virtual void Initialize(GLFWwindow* winndow) { Initialize(); }

	virtual void Update(double deltaTime) {}
	virtual void Render(float aspectRation) {}

	virtual void ClearResources() {}
};