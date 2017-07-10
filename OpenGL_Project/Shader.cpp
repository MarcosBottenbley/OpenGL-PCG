#include "Shader.h"

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
{
	this->vertexPath = const_cast<GLchar*>(vertexPath);
	this->fragmentPath = const_cast<GLchar*>(fragmentPath);

	GetUniformNames();

	std::string vertexCode;
	std::string fragmentCode;

	ReadFiles(vertexCode, fragmentCode);

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	CompileShaders(vShaderCode, fShaderCode);
}

Shader::Shader(const GLchar * vertexPath, const GLchar * geometryPath, const GLchar * fragmentPath)
{
	this->vertexPath = const_cast<GLchar*>(vertexPath);
	this->geometryPath = const_cast<GLchar*>(geometryPath);
	this->fragmentPath = const_cast<GLchar*>(fragmentPath);

	std::string vertexCode;
	std::string geometryCode;
	std::string fragmentCode;

	ReadFiles(vertexCode, geometryCode, fragmentCode);

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	CompileShaders(vShaderCode,gShaderCode, fShaderCode);
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

void Shader::ReadFiles(std::string & vertexCode, std::string & geometryCode, std::string fragmentCode)
{
	std::ifstream vShaderFile;
	std::ifstream gShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		gShaderFile.open(geometryPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, gShaderSteam, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		gShaderSteam  << gShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		gShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		geometryCode = gShaderSteam.str();
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
	GLuint vertexShader = CompileVertexShader(vShaderCode, success, infoLog);

	//compiling fragment shader
	GLuint fragmentShader = CompileFragmentShader(fShaderCode, success, infoLog);

	//creating a shader program and attaching compiled shaders to program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertexShader);
	glAttachShader(this->Program, fragmentShader);

	glLinkProgram(this->Program);
	CheckForCompilerErrors(this->Program, "PROGRAM");

	//deleting shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::CompileShaders(const GLchar * vShaderCode, const GLchar * gShaderCode, const GLchar * fShaderCode)
{
	//variables for error checking
	GLint success;
	GLchar infoLog[512];

	//compiling vertex shader
	GLuint vertexShader = CompileVertexShader(vShaderCode, success, infoLog);

	//compiling fragment shader
	GLuint fragmentShader = CompileFragmentShader(fShaderCode, success, infoLog);

	//compiling geometry shader
	GLuint geometryShader = CompileGeometryShader(gShaderCode, success, infoLog);

	//creating a shader program and attaching compiled shaders to program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertexShader);
	glAttachShader(this->Program, geometryShader);
	glAttachShader(this->Program, fragmentShader);

	glLinkProgram(this->Program);
	CheckForCompilerErrors(this->Program, "PROGRAM");

	//deleting shaders
	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
}

GLuint Shader::CompileVertexShader(const GLchar * vShaderCode, GLint& success, GLchar* infoLog)
{
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);
	CheckForCompilerErrors(vertexShader, "VERTEX");
	return vertexShader;
}

GLuint Shader::CompileGeometryShader(const GLchar * gShaderCode, GLint& success, GLchar* infoLog)
{
	GLuint geometryShader;
	geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &gShaderCode, NULL);
	glCompileShader(geometryShader);
	CheckForCompilerErrors(geometryShader, "GEOMETRY");
	return geometryShader;
}

GLuint Shader::CompileFragmentShader(const GLchar * fShaderCode, GLint& success, GLchar* infoLog)
{	
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);
	CheckForCompilerErrors(fragmentShader, "FRAGMENT");
	return fragmentShader;
}

void Shader::CheckForCompilerErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR " << type << " SHADER COMPILATION FAILED\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR SHADER " << type << " LINKING FAILED\n" << infoLog << std::endl;
		}
	}
}
