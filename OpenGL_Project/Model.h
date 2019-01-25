#pragma once

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "Glm_Common.h"

class Model
{
public:
	Model(const GLchar* modelPath);
	Model(const GLchar* modelPath, const int count, const GLenum usage);
	~Model();
	void bind();
	void unbind();
	GLuint getVertexCount();
	GLuint getNormalCount();
	GLuint getIndexCount();

	void updateVBO(GLuint vbo, std::vector<Matrix4> data);

	GLuint addEmptyVBO(int count, GLenum usage);

private:
	template <typename Type>
	GLuint addVBO(std::vector<Type> data, GLenum usage);

	void addEBO(GLenum usage);

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLuint>  indices;

	std::vector<GLuint> VBOS;
	GLuint VAO, EBO;
};

