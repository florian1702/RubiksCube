#include "TestSound.h"
#include <windows.h>
#include <GLFW/glfw3.h>

void TestSound::Initialize(GLFWwindow* window) {
	m_input.SetWindow(window);
	m_input.ObserverKey(GLFW_KEY_SPACE);

}

void TestSound::Update(double deltaTime) {
	m_input.Update();
	if (m_input.WasKeyPressed(GLFW_KEY_SPACE))
		PlaySound(TEXT("Hammer.wav"), NULL, SND_FILENAME | SND_ASYNC);
}