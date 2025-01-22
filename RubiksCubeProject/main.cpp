#define GLEW_STATIC
#include "IGameInterface.h"
#include "RubiksGameInterface.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

// GAME INTERFACE INSTANCES
RubiksGameInterface gRubiksGameInterface;
IGameInterface* gUsedInterface;

// TIMING VARIABLES
double lastTime = glfwGetTime();
double deltaTime = 0.0;

// RENDERS THE CONTENT OF THE WINDOW
void RenderWindow(GLFWwindow* window) {
    gUsedInterface->Update(deltaTime); // UPDATE GAME LOGIC WITH DELTA TIME

    // GET WINDOW SIZE AND CALCULATE ASPECT RATIO
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    // CONFIGURE VIEWPORT AND ENABLE DEPTH TESTING
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // CLEAR THE SCREEN WITH A BACKGROUND COLOR
    float bgColor[3] = { 0.2f, 0.2f, 0.2f };
    glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // RENDER ONLY IF THE WINDOW IS NOT MINIMIZED
    if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
        gUsedInterface->Render(aspectRatio);  // DELEGATE RENDERING TO THE GAME INTERFACE
    }

    // SWAP THE FRONT AND BACK BUFFERS
    glfwSwapBuffers(window);

    // CALCULATE DELTA TIME FOR THIS FRAME
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // LIMIT FRAME RATE BY PAUSING FOR 8 MS
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
}

// MAIN LOOP: PROCESSES EVENTS AND RENDERS THE FRAME
void RunCoreLoop(GLFWwindow* window) {
    glfwPollEvents();       // PROCESS ALL WINDOW EVENTS
    RenderWindow(window);   // RENDER THE CURRENT FRAME
}

// INITIALIZES GLFW, GLEW, AND CREATES A WINDOW
GLFWwindow* InitializeSystem() {
    glfwInit(); // INITIALIZE THE GLFW LIBRARY

    // CONFIGURE THE OPENGL CONTEXT VERSION (3.3 CORE PROFILE)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // CREATE THE APPLICATION WINDOW
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Rubiks Cube", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetWindowRefreshCallback(window, RenderWindow);

    // INITIALIZE GLEW
    glewExperimental = true;  // ENSURE GLEW USES MODERN OPENGL FUNCTIONS
    glewInit();               // LOAD OPENGL FUNCTION POINTERS

    // INITIALIZE THE GAME INTERFACE
    gUsedInterface->Initialize(window);
    return window;
}

// RELEASES RESOURCES AND TERMINATES GLFW
void ShutdownSystem() {
    gUsedInterface->ClearResources();
    glfwTerminate();
}

// PROGRAM ENTRY POINT
int main() {
    gUsedInterface = &gRubiksGameInterface;      // SET THE ACTIVE GAME INTERFACE
    GLFWwindow* window = InitializeSystem();     // INITIALIZE SYSTEM COMPONENTS

    // MAIN LOOP: RUN UNTIL THE WINDOW IS CLOSED
    while (!glfwWindowShouldClose(window)) { 
        RunCoreLoop(window);
    }

    // CLEAN UP AND EXIT
    ShutdownSystem();
}
