#include "Model.h"



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