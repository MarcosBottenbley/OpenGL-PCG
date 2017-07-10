#include "TerrainGenerator.h"



TerrainGenerator::TerrainGenerator(int chunkSize)
{
	this->chunkSize = chunkSize;
	chunksVisibleInViewDist = maxViewDist / chunkSize;
}


TerrainGenerator::~TerrainGenerator()
{
}
