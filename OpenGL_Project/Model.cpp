#include "Model.h"

#include "Glm_Common.h"
#include "Perlin.h"


Model::Model(const GLchar* modelPath)
{
	std::fstream myfile;
	myfile.open(modelPath);

	bool vertex = false;
	bool normal = false;
	bool index = false;

	std::string line;
	while (std::getline(myfile, line))
	{
		std::istringstream in_floats(line);
		std::istringstream in_string(line);

		std::string line;
		in_string >> line;
		if (line == "vertices")
		{
			vertex = true;
			normal = false;
			index  = false;
		}
		else if (line == "normals")
		{
			vertex = false;
			normal = true;
			index  = false;
		}
		else if (line == "indices")
		{
			vertex = false;
			normal = false;
			index  = true;
		} 
		else if (vertex)
		{
			GLfloat x, y, z;
			in_floats >> x >> y >> z;
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
		else if (normal)
		{
			GLfloat x, y, z;
			in_floats >> x >> y >> z;
			normals.push_back(x);
			normals.push_back(y);
			normals.push_back(z);
		}
		else if (index)
		{
			GLuint x, y, z;
			in_floats >> x >> y >> z;
			indices.push_back(x);
			indices.push_back(y);
			indices.push_back(z);
		}
	}
	vertices.shrink_to_fit();
	normals.shrink_to_fit();
	indices.shrink_to_fit();

	glGenVertexArrays(1, &VAO);
	bind();
	addVBO(vertices);
	addEBO();
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	
	if (!normals.empty())
	{
		addVBO(normals);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
	}

	const int chunkSize = 500;
	const int chunkHeight = 16;
	Perlin* p = new Perlin();
	int octaves = 6;
	double persistence = 0.25;
	double lacunarity = 2.0;
	double scale = 1.0;
	std::vector<Matrix4> terrainModels;

	for (double r = 0; r < chunkSize; r++)
	{
		for (double c = 0; c < chunkSize; c++)
		{
			double getnoise = 0;
			getnoise = p->OctavePerlin(abs(r) / 30, abs(c) / 30, 0, octaves, persistence, lacunarity, scale);
			Matrix4 model;
			model = glm::translate(model, Vector3((GLfloat)r, (GLfloat)getnoise * chunkHeight, (GLfloat)c));
			terrainModels.push_back(model);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, terrainModels.size() * sizeof(terrainModels[0]), terrainModels.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)(3 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);

	unbind();
}


Model::~Model()
{
	glDeleteBuffers((GLsizei) VBOS.size(), VBOS.data());
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
}

void Model::bind()
{
	glBindVertexArray(VAO);
}

void Model::unbind()
{
	glBindVertexArray(0);
}

GLuint Model::getVertexCount()
{
	return (GLuint) vertices.size();
}

GLuint Model::getNormalCount()
{
	return (GLuint) normals.size();
}

GLuint Model::getIndexCount()
{
	return (GLuint) indices.size();
}

void Model::addVBO(std::vector<GLfloat> data)
{
	GLuint VBO;
	VBOS.push_back(VBO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
}

void Model::addEBO()
{
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
}