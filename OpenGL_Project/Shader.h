#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL\glew.h>

class Shader
{
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Shader();

	GLuint Program;
	void Use();
private:
	void compileShaders(const GLchar* vShaderCode, const GLchar* fShaderCode);
};
#endif