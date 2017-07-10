#pragma once

#include <math.h>
#include "Glm_Common.h"

class TerrainGenerator
{
public:
	TerrainGenerator(int chunkSize);
	~TerrainGenerator();
private:
	const float maxViewDist = 300;
	int chunkSize;
	int chunksVisibleInViewDist;
	Vector2 viewerPosition;
};

