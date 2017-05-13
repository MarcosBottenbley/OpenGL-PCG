#pragma once

#include <GL\glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class Model
{
public:
	Model(const GLchar* modelPath);
	~Model();
	void bind();
	void unbind();
	GLuint getVertexCount();
	GLuint getIndexCount();

private:
	void addVBO();
	void addEBO();
	void addVAO();

	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	GLuint vertexCount =  0, indexCount = 0;
	GLuint VBO, VAO, EBO;
};

