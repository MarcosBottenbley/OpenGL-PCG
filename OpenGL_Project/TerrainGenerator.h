#pragma once

#include <math.h>
#include <vector>
#include <unordered_map>
#include <glad\glad.h>

#include "Glm_Common.h"
#include "Model.h"
#include "Perlin.h"

struct pair_hash
{
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2> &p) const
	{
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);

		// Mainly for demonstration purposes, i.e. works but is overly simple
		// In the real world, use sth. like boost.hash_combine
		return h1 ^ h2;
	}
};

class TerrainGenerator
{
public:
	TerrainGenerator(Model& meshObject, int chunkSize, int viewX, int viewZ);
	~TerrainGenerator();

	void updateViewPosition(int viewX, int viewZ);

private:
	//variables
	const int octaves = 6;
	const double persistence = 0.25;
	const double lacunarity = 5.0;
	double scale = 10.0;
	int chunkHeight = 70;

	int chunkSize;		//length and width of the chunks in blocks

	std::unordered_map<std::pair<double,double>, Matrix4, pair_hash> myMap;

	Vector2 viewPosition;
	std::vector<Matrix4> visibleChunks;

	Model* meshObject = nullptr;
	Perlin* p;
	GLuint instanceVBO;

	//functions
	void UpdateVisibleChunks();
	void GenerateChunk(int centerX, int centerY);
};

