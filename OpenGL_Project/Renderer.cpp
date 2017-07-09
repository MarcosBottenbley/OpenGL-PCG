#include "Renderer.h"

Renderer::Renderer(Model & model, Shader & shader)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	this->model = &model;
	this->shader = &shader;
}

Renderer::~Renderer()
{
	delete model;
	delete shader;
}

void Renderer::prepare()
{
	//render and clear the color buffer
	glClearColor(skyColor.x, skyColor.y, skyColor.z, skyColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}