#pragma once
#include "Renderer.h"
class BlockRenderer :
	public Renderer
{
public:
	BlockRenderer(Model& model, Shader& shader);
	~BlockRenderer();
	void getUniformLocations();
private:
	//fragment shader uniform locations
	GLuint lightPosLocation;
	GLuint lightColorLocation;
	GLuint objectColorLocation;
	GLuint skyColorLocation;

	//vertex shader uniform locations
	GLuint modelLocation;
	GLuint viewLocation;
	GLuint projectionLocation;
};