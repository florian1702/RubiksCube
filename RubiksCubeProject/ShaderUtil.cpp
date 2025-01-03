
//FERTIG

#define STB_IMAGE_IMPLEMENTATION
#include "ShaderUtil.h"
#include <stb_image.h>
#include <iostream>
#include <fstream>

GLuint ShaderUtil::CreateShaderProgram(const char* vertexFilename, const char* fragmentFilename)
{
	std::string vertexProgram = LoadFile(vertexFilename);
	const char* vertexString = vertexProgram.c_str();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexString, NULL);
	PrintOGlErrors();
	glCompileShader(vertexShader);
	PrintOGlErrors();
	int vertCompiled;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1)
	{
		std::cout << "Vertex shader error " << std::endl;
		PrintShaderLog(vertexShader);
	}


	std::string fragmentProgram = LoadFile(fragmentFilename);
	const char* fragmentString = fragmentProgram.c_str();
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentString, NULL);
	
	PrintOGlErrors();
	glCompileShader(fragmentShader);
	PrintOGlErrors();
	int fragCompiled;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1)
	{
		std::cout << "Fragment shader error " << std::endl;
		PrintShaderLog(fragmentShader);
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	PrintOGlErrors();
	int linked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
	if (linked != 1)
	{
		std::cout << "Linking failed:" << std::endl;
		PrintProgramLog(shaderProgram);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	return shaderProgram;	
}

GLuint ShaderUtil::LoadTexture(const std::string& filePath)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("Texture.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}
	stbi_image_free(data);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

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

void ShaderUtil::PrintShaderLog(GLuint shader)
{
	int memLength;
	int messageLength;
	char* log;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &memLength);
	if (memLength == 0)
		return;
	log = (char*)malloc(memLength);
	glGetShaderInfoLog(shader, memLength, &messageLength, log);
	std::cout << "Shader Info Log: " << log << std::endl;
	free(log);
}

void ShaderUtil::PrintProgramLog(GLuint program)
{
	int memLength;
	int messageLength;
	char* log;
	
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &memLength);
	if (memLength == 0)
		return;
	log = (char*)malloc(memLength);
	glGetProgramInfoLog(program, memLength, &messageLength, log);
	std::cout << "Program Info Log: " << log << std::endl;
	free(log);
}

void ShaderUtil::PrintOGlErrors()
{
	int error = glGetError();
	while (error != GL_NO_ERROR)
	{
		std::cout << "OpenGL error: " << error << std::endl;
		error = glGetError();
	}
}

