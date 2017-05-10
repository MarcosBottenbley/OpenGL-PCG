#pragma once
#ifndef PERLIN_H
#define PERLIN_H

#include <GL\glew.h>
#include <glm\glm.hpp>

#include <vector>
#include <iostream>

class Perlin
{
public:
	//takes in a width (size of x) and sets length to same value
	//all maps are square maps
	//generates an array of glm::vec3 with length width * height
	Perlin(GLuint width);
	~Perlin();
	glm::vec3 * cubePositions();

private:
	std::vector<int> generateFrequencyVector(int maxFrequency);
	void gradiant();
	void createLattice(GLuint frequency);
	double lerp(double a, double b, double x) { return a + x * (b - a); }

	GLuint WIDTH = 0;
	GLuint LENGTH = 0;
};
#endif