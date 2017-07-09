#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL\glew.h>

class Shader
{
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Shader();

	GLuint Program;
	void Use();
	std::vector<std::string>& GetUniformNames();
private:
	//vars
	const GLchar * vertexPath = nullptr;
	const GLchar * fragmentPath = nullptr;

	//methods
	void ReadFiles(std::string& vertexCode, std::string& fragmentCode);
	void CompileShaders(const GLchar* vShaderCode, const GLchar* fShaderCode);
};