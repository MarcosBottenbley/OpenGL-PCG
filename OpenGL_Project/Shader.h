#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad\glad.h>

#include "Glm_Common.h"

class Shader
{
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);
	~Shader();

	GLuint Program;
	void Use();
	std::vector<std::string>& GetUniformNames();

	//set uniform methods
	void SetUniform(const std::string& name, Vector3& value) const
	{
		glUniform3f(glGetUniformLocation(this->Program, name.c_str()), value.x, value.y, value.z);
	}
	void SetUniform(const std::string& name, Vector4& value) const
	{
		glUniform4f(glGetUniformLocation(this->Program, name.c_str()), value.x, value.y, value.z, value.w);
	}
	void SetUniform(const std::string& name, Matrix4& value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(this->Program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

private:
	//vars
	const GLchar * vertexPath = nullptr;
	const GLchar * geometryPath = nullptr;
	const GLchar * fragmentPath = nullptr;

	//methods
	void ReadFiles(std::string& vertexCode, std::string& fragmentCode);
	void ReadFiles(std::string& vertexCode, std::string& geometryCode, std::string fragmentCode);

	void CompileShaders(const GLchar* vShaderCode, const GLchar* fShaderCode);
	void CompileShaders(const GLchar* vShaderCode, const GLchar* gShaderCode, const GLchar* fShaderCode);

	GLuint CompileVertexShader(const GLchar* vShaderCode, GLint& success, GLchar* infoLog);
	GLuint CompileGeometryShader(const GLchar* gShaderCode, GLint& success, GLchar* infoLog);
	GLuint CompileFragmentShader(const GLchar* fShaderCode, GLint& success, GLchar* infoLog);

	void CheckForCompilerErrors(GLuint shader, std::string type);
};