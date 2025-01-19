#define GLEW_STATIC
#include "IGameInterface.h"
#include "RubiksGameInterface.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

// Game interface and implementation instance
RubiksGameInterface gRubiksGameInterface;
IGameInterface* gUsedInterface;

// Timing variables
double lastTime = glfwGetTime();
double deltaTime = 0.0;

// Renders the window content
void RenderWindow(GLFWwindow* window) {
    gUsedInterface->Update(deltaTime); // Update game logic

    // Get window size and calculate aspect ratio
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    // Set viewport and enable depth testing
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Clear the screen with a background color
    float bgColor[3] = { 0.2f, 0.2f, 0.2f };
    glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render only if the window is not minimized (without it crashed)
    if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
        gUsedInterface->Render(aspectRatio); 
    }

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Calculate delta time for the current frame
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Pause the thread for 8 ms -> lower frame rate (slower animations)
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
}

// Main loop for processing events and rendering
void RunCoreLoop(GLFWwindow* window) {
    glfwPollEvents();       
    RenderWindow(window);
}

// Initializes GLFW, GLEW, and creates a window
GLFWwindow* InitializeSystem() {
    glfwInit(); // Initialize GLFW

    // Specify OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // Create the window
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Rubiks Cube", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetWindowRefreshCallback(window, RenderWindow);

    // Initialize GLEW
    glewExperimental = true;
    glewInit();

    // Initialize the game interface
    gUsedInterface->Initialize(window);
    return window;
}

// Shuts down the system and releases resources
void ShutdownSystem() {
    gUsedInterface->ClearResources();
    glfwTerminate();
}

// Program entry point
int main() {
    gUsedInterface = &gRubiksGameInterface;      // Set the active game interface
    GLFWwindow* window = InitializeSystem();     // Initialize the system

    while (!glfwWindowShouldClose(window)) {     // Main loop until the window is closed
        RunCoreLoop(window);
    }

    ShutdownSystem();                            // Clean up and exit
}
