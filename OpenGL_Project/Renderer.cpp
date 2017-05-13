#include "Renderer.h"



Renderer::Renderer(Shader* shader, Camera* camera)
{
	this->shader = shader;
	this->camera = camera;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}


Renderer::~Renderer()
{
	delete shader;
}

void Renderer::update()
{
	shader->Use();
	setUniforms(800.0f, 600.0f);
}

void Renderer::clear()
{
	glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setUniforms(GLfloat width, GLfloat height)
{
	//fill view uniform
	glm::mat4 view;
	view = camera->getViewMatrix();
	GLuint viewLoc = glGetUniformLocation(shader->Program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//fill projection uniform
	glm::mat4 projection;
	projection = glm::perspective(45.0f, width / height, 0.1f, 100.0f);
	GLuint projectionLoc = glGetUniformLocation(shader->Program, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}
