#pragma once
#ifndef PERLIN_H
#define PERLIN_H

#include <GL\glew.h>
#include <glm\glm.hpp>

class Perlin
{
public:
	Perlin();
	~Perlin();

	glm::vec3 cubePositions[];
	GLfloat perlin(GLfloat x, GLfloat y, GLfloat z);
};
#endif