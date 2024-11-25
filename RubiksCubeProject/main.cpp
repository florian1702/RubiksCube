
#define GLEW_STATIC
#include "IGameInterface.h"
#include "RubiksGameInterface.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>


RubiksGameInterface gRubiksGameInterface;
IGameInterface* gUsedInterface;

double lastTime = glfwGetTime();
double deltaTime = 0.0;


void RenderWindow(GLFWwindow* window) {

	gUsedInterface->Update(deltaTime);
	//gUsedInterface->Update(0.1); // fürs Debuggen
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	//BACKGROUND COLOR
	float bgColor[3] = { 0.2, 0.2, 0.2 };

	glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Only render when not minimized else error
	int minimized = glfwGetWindowAttrib(window, GLFW_ICONIFIED);
	if (!minimized) {
			gUsedInterface->Render(aspectRatio);
	}

	glfwSwapBuffers(window);


	double currenTime = glfwGetTime();
	deltaTime = currenTime - lastTime;
	lastTime = currenTime;
	std::this_thread::sleep_for(std::chrono::milliseconds(8));

}

void RunCoreLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		RenderWindow(window);
	}
}

GLFWwindow* InitializeSystem() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "Rubiks Cube", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetWindowRefreshCallback(window, RenderWindow);

	glewExperimental = true;
	glewInit();
	gUsedInterface->Initialize(window);

	return window;
}

void ShutdownSystem() {
	gUsedInterface->ClearResources();
	glfwTerminate();

}

int main()
{
	gUsedInterface = &gRubiksGameInterface;
	GLFWwindow* window = InitializeSystem();
	RunCoreLoop(window);
	ShutdownSystem();
}