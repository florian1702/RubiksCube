#define STB_IMAGE_IMPLEMENTATION
#include "ShaderUtil.h"
#include <stb_image.h>
#include <iostream>
#include <fstream>

// LOAD AND COMPILE VERTEX AND FRAGMENT SHADERS, CREATE SHADER PROGRAM
GLuint ShaderUtil::CreateShaderProgram(const char* vertexFilename, const char* fragmentFilename)
{
	// LOAD VERTEX SHADER SOURCE FROM FILE
	std::string vertexProgram = LoadFile(vertexFilename);
	const char* vertexString = vertexProgram.c_str();

	// CREATE AND COMPILE VERTEX SHADER
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexString, NULL);
	PrintOGlErrors();
	glCompileShader(vertexShader);
	PrintOGlErrors();

	// CHECK VERTEX SHADER COMPILATION STATUS
	int vertCompiled;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1)
	{
		std::cout << "Vertex shader error " << std::endl;
		PrintShaderLog(vertexShader);
	}

	// LOAD FRAGMENT SHADER SOURCE FROM FILE
	std::string fragmentProgram = LoadFile(fragmentFilename);
	const char* fragmentString = fragmentProgram.c_str();

	// CREATE AND COMPILE FRAGMENT SHADER
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentString, NULL);
	PrintOGlErrors();
	glCompileShader(fragmentShader);
	PrintOGlErrors();

	// CHECK FRAGMENT SHADER COMPILATION STATUS
	int fragCompiled;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1)
	{
		std::cout << "Fragment shader error " << std::endl;
		PrintShaderLog(fragmentShader);
	}

	// CREATE SHADER PROGRAM AND LINK SHADERS
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	PrintOGlErrors();

	// CHECK LINKING STATUS
	int linked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
	if (linked != 1)
	{
		std::cout << "Linking failed:" << std::endl;
		PrintProgramLog(shaderProgram);
	}

	// DELETE INDIVIDUAL SHADERS AFTER LINKING
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	return shaderProgram;	
}

// LOAD AND SET UP A 2D TEXTURE
GLuint ShaderUtil::LoadTexture(const char* textureFilename)
{
	// GENERATE AND BIND TEXTURE
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// LOAD IMAGE DATA USING STB_IMAGE
	int width, height, nrChannels;
	unsigned char* data = stbi_load(textureFilename, &width, &height, &nrChannels, 0);
	if (data) {
		// UPLOAD TEXTURE DATA AND GENERATE MIPMAPS
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}
	stbi_image_free(data);

	// SET TEXTURE PARAMETERS
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

// READ A FILE AND RETURN ITS CONTENT AS A STRING
std::string ShaderUtil::LoadFile(const char* fileName)
{
	std::string result;
	std::ifstream fileStream(fileName, std::ios::in);
	while (!fileStream.eof())
	{
		std::string line;
		std::getline(fileStream, line);
		result.append(line + "\n");
	}
	fileStream.close();
	return result;
}

// PRINT LOG INFORMATION FOR A SHADER OBJECT
void ShaderUtil::PrintShaderLog(GLuint shader)
{
	int memLength;
	int messageLength;
	char* log;

	// GET LOG LENGTH AND RETRIEVE LOG
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &memLength);
	if (memLength == 0)
		return;
	log = (char*)malloc(memLength);
	glGetShaderInfoLog(shader, memLength, &messageLength, log);
	std::cout << "Shader Info Log: " << log << std::endl;
	free(log);
}

// PRINT LOG INFORMATION FOR A SHADER PROGRAM
void ShaderUtil::PrintProgramLog(GLuint program)
{
	int memLength;
	int messageLength;
	char* log;

	// GET LOG LENGTH AND RETRIEVE LOG
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &memLength);
	if (memLength == 0)
		return;
	log = (char*)malloc(memLength);
	glGetProgramInfoLog(program, memLength, &messageLength, log);
	std::cout << "Program Info Log: " << log << std::endl;
	free(log);
}

// PRINT ALL OPENGL ERRORS
void ShaderUtil::PrintOGlErrors()
{
	int error = glGetError();
	while (error != GL_NO_ERROR)
	{
		std::cout << "OpenGL error: " << error << std::endl;
		error = glGetError();
	}
}

