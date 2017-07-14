#include "TerrainGenerator.h"



TerrainGenerator::TerrainGenerator(Model& meshObject, int chunkSize, int viewX, int viewZ)
{
	this->meshObject = &meshObject;
	this->chunkSize = chunkSize;

	p = new Perlin();
	viewPosition = Vector2(viewX, viewZ);
	GenerateChunk(viewX, viewZ);

	meshObject.bind();
	instanceVBO = meshObject.addEmptyVBO(visibleChunks.size(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (GLvoid*)(sizeof(Vector4)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (GLvoid*)(2 * sizeof(Vector4)));
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (GLvoid*)(3 * sizeof(Vector4)));
	glEnableVertexAttribArray(5);

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	meshObject.unbind();
	UpdateVisibleChunks();
	meshObject.updateVBO(instanceVBO,visibleChunks);
}


TerrainGenerator::~TerrainGenerator()
{
	delete p;
}

void TerrainGenerator::updateViewPosition(int viewX, int viewZ)
{
	float squareDist = powf(viewPosition.x - viewX, 2) + powf(viewPosition.y - viewZ, 2);
	float dist = sqrt(squareDist);
	if (dist > chunkSize / 4)
	{
		viewPosition = Vector2(viewX, viewZ);
		GenerateChunk(viewX, viewZ);
		UpdateVisibleChunks();
		this->meshObject->updateVBO(instanceVBO, visibleChunks);
	}
}

void TerrainGenerator::UpdateVisibleChunks()
{
	visibleChunks.clear();
	std::vector<std::pair<double, double>> eraseable;
	for (auto& x: myMap)
	{
		if (x.first.first < (-chunkSize / 2) + viewPosition.x || x.first.first > (chunkSize / 2) + viewPosition.x)
		{
			eraseable.push_back(x.first);
		} 
		else if (x.first.second < (-chunkSize / 2) + viewPosition.y || x.first.second > (chunkSize / 2) + viewPosition.y)
		{
			eraseable.push_back(x.first);
		}
		else
			visibleChunks.push_back(x.second);
	}
	for (auto& y : eraseable)
	{
		myMap.erase(y);
	}
	eraseable.clear();
}

void TerrainGenerator::GenerateChunk(int centerX, int centerY)
{
	for (double r = (-chunkSize / 2) + centerX; r < (chunkSize / 2) + centerX; r++)
	{
		for (double c = (-chunkSize / 2) + centerY; c < (chunkSize / 2) + centerY; c++)
		{
			std::pair<double, double> key = std::make_pair(r, c);
			if (myMap.count(key) == 0)
			{
				double getnoise = 0;
				getnoise = p->OctavePerlin(abs(r) / 30, abs(c) / 30, 0, octaves, persistence, lacunarity, scale);
				Matrix4 model;
				model = glm::translate(model, Vector3((GLfloat)r, (GLfloat)getnoise * chunkHeight, (GLfloat)c));
				myMap[key] = model;
			}
		}
	}
}