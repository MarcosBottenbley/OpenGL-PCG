#include "Model.h"



Model::Model(const GLchar* modelpath)
{
	//read vertices and indices from file
	/*
	std::ifstream inFile;
	inFile.open(modelpath);
	if (inFile.fail())
	{
		std::cerr << "Error opening file" << std::endl;
	}
	*/

	vertices = {
		//back
		0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,

		//right
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,

		//front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		//left
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,

		//top
		-0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		//bottom
		-0.5f, -0.5f, -0.5f,	//front left - 16
		0.5f, -0.5f, -0.5f,	//front right - 17
		0.5f, -0.5f,  0.5f,	//back right - 18
		-0.5f, -0.5f,  0.5f		//back left - 19
	};


	indices = {
		//back
		0, 1, 2,
		2, 3, 0,

		//front
		4, 5, 6,
		6, 7, 4,

		//left
		8, 9, 10,
		10, 11, 8,

		//right
		12, 13, 14,
		14, 15, 12,

		//bottom
		16, 17, 18,
		18, 19, 16,

		//top
		20, 21, 22,
		22, 23, 20
	};

	//generating VAOs
	glGenVertexArrays(1, &VAO);
	bind();
	addVBO();
	addEBO();
	addVAO();
	unbind();
}


Model::~Model()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Model::bind()
{
	glBindVertexArray(VAO);
}

void Model::unbind()
{
	//unbind GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint Model::getVertexCount()
{
	vertexCount = vertices.size();
	return vertexCount;
}

GLuint Model::getIndexCount()
{
	indexCount = indices.size();
	return indexCount;
}

void Model::addVBO()
{
	//generating VBOs
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertexCount, vertices.data(), GL_STATIC_DRAW);
}

void Model::addEBO()
{
	//generating EBOs
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexCount, indices.data(), GL_STATIC_DRAW);
}

void Model::addVAO()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
}
