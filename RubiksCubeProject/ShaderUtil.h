#pragma once
#include <GL/glew.h>
#include <string>

// SHADERUTIL CLASS: PROVIDES METHODS FOR LOADING SHADERS, TEXTURES, AND HANDLING ERRORS
class ShaderUtil {
public:
	static GLuint CreateShaderProgram(const char* vertexFilename, const char* fragmentFilename);
	static GLuint LoadTexture(const char* textureFilename);

private:
	static std::string LoadFile(const char* fileName);
	static void PrintShaderLog(GLuint shader);
	static void PrintProgramLog(GLuint program);
	static void PrintOGlErrors();
};