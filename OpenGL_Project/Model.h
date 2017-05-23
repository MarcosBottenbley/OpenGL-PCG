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
	GLuint getNormalCount();
	GLuint getIndexCount();

private:
	void addVBO(std::vector<GLfloat> data);
	void addEBO();

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLuint>  indices;

	std::vector<GLuint> VBOS;
	GLuint VAO, EBO;
};

