#pragma once

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
	Perlin();
	double OctavePerlin(double x, double y, double z, int octaves, double persistence);
	double perlin(double x, double y, double z);
	int inc(int num);
	double grad(int hash, double x, double y, double z);
	double fade(double t);
	double lerp(double a, double b, double x);

	int repeat;

private:
	//static int p[512];
};