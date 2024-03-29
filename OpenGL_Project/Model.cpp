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
	//add the vertex data
	addVBO(vertices, GL_STATIC_DRAW);

	//add the index data
	addEBO(GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	
	//if there are normals add the normals
	if (!normals.empty())
	{
		addVBO(normals, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
	}
	unbind();
}

Model::Model(const GLchar * modelPath, const int count, const GLenum usage) 
	:Model(modelPath)
{

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

void Model::updateVBO(GLuint vbo, std::vector<Matrix4> data)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(data[0]), data.data());
}

template <typename Type>
GLuint Model::addVBO(std::vector<Type> data, GLenum usage)
{
	GLuint VBO;
	VBOS.push_back(VBO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(), usage);
	return VBO;
}

GLuint Model::addEmptyVBO(int count, GLenum usage)
{
	GLuint emptyVBO;
	VBOS.push_back(emptyVBO);
	glGenBuffers(1, &emptyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, emptyVBO);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(Matrix4), NULL, usage);
	return emptyVBO;
}

void Model::addEBO(GLenum usage)
{
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), usage);
}