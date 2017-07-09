#include "BlockRenderer.h"



BlockRenderer::BlockRenderer(Model& model, Shader& shader)
	:Renderer(model, shader)
{
	//TODO: make the model and shader private variables
}


BlockRenderer::~BlockRenderer()
{
}

void BlockRenderer::getUniformLocations()
{
	modelLocation = glGetUniformLocation(shader->Program, "model");
	viewLocation = glGetUniformLocation(shader->Program, "view");
	projectionLocation = glGetUniformLocation(shader->Program, "projection");
}
