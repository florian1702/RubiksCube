//FERTIG


#pragma once
#include <GL/glew.h>
#include <string>

class ShaderUtil {
public:
	static GLuint CreateShaderProgram(const char* vertexFilename, const char* fragmentFilename);
	static GLuint LoadTexture(const std::string& filePath);

private:
	static std::string LoadFile(const char* fileName);
	static void PrintShaderLog(GLuint shader);
	static void PrintProgramLog(GLuint program);
	static void PrintOGlErrors();
	
};

