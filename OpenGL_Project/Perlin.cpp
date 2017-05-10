#include "Perlin.h"

Perlin::Perlin(GLuint width)
{
	WIDTH = width;
	LENGTH = width;
	std::vector<int> freqVec = generateFrequencyVector(width);
}

Perlin::~Perlin()
{
}

glm::vec3 * Perlin::cubePositions()
{
	return nullptr;
}

std::vector<int> Perlin::generateFrequencyVector(int maxFrequency)
{
	std::vector<int> freqVector;
	for (int x = 0; x < maxFrequency; x++)
	{
		if ()
	}
	return freqVector;
}

void Perlin::gradiant()
{
}

void Perlin::createLattice(GLuint frequency)
{
}
