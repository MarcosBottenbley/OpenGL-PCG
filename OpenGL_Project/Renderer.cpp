#include "Renderer.h"



Renderer::Renderer()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


Renderer::~Renderer()
{
}

void Renderer::prepare()
{
	Vector4 skyColor = Vector4(0.2f, 0.3f, 0.5f, 1.0f);
	//render and clear the color buffer
	glClearColor(skyColor.x, skyColor.y, skyColor.z, skyColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(Model model)
{
}
