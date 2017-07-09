#include "Shader.h"

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
{
	this->vertexPath = const_cast<GLchar*>(vertexPath);
	this->fragmentPath = const_cast<GLchar*>(fragmentPath);

	std::string vertexCode;
	std::string fragmentCode;

	ReadFiles(vertexCode, fragmentCode);

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	CompileShaders(vShaderCode, fShaderCode);
}

Shader::~Shader()
{
}

void Shader::Use()
{
	glUseProgram(this->Program);
}

std::vector<std::string>& Shader::GetUniformNames()
{
	std::vector<std::string> uniformNames;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);

	std::istringstream iss;
	std::string line;
	std::string word;
	while (std::getline(vShaderFile, line))
	{
		iss.str (line);
		while (iss >> word)
		{
			if (word == "uniform")
			{
				iss >> word;
				iss >> word;
				uniformNames.push_back(word.substr(0, word.size() - 1));
			}
		}
		iss.clear();
	}

	line = "";
	while (std::getline(fShaderFile, line))
	{
		iss.str (line);
		while (iss >> word)
		{
			if (word == "uniform")
			{
				iss >> word;
				iss >> word;
				uniformNames.push_back(word.substr(0, word.size() - 1));
			}
		}
		iss.clear();
	}

	vShaderFile.close();
	fShaderFile.close();

	return uniformNames;
}

void Shader::ReadFiles(std::string & vertexCode, std::string & fragmentCode)
{
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR SHADER FILE NOT SUCCESSFULLY READ" << std::endl;
	}
}

void Shader::CompileShaders(const GLchar * vShaderCode, const GLchar * fShaderCode)
{
	//variables for error checking
	GLint success;
	GLchar infoLog[512];

	//compiling vertex shader
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR VERTEX SHADER COMPILATION FAILED\n" << infoLog << std::endl;
	}

	//compiling fragment shader
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR FRAGMENT SHADER COMPILATION FAILED\n" << infoLog << std::endl;
	}

	//creating a shader program and attaching compiled shaders to program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertexShader);
	glAttachShader(this->Program, fragmentShader);
	glLinkProgram(this->Program);
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		std::cout << "ERROR SHADER PROGRAM LINKING FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}