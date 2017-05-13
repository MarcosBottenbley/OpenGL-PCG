#include "BlockRenderer.h"



BlockRenderer::BlockRenderer(Camera* camera)
	:Renderer(&Shader("Shaders/vertexShader.glsl", "Shaders/fragmentShader.glsl"), camera)
{
	block = new Model("path/to/block.model");
}

BlockRenderer::~BlockRenderer()
{
	delete block;
}

void BlockRenderer::draw()
{
	glDrawElements(GL_TRIANGLE_FAN, block->getVertexCount(), GL_UNSIGNED_INT, nullptr);
	//draw lines
	setColorUniform(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	glLineWidth(3.0f);
	glDrawElements(GL_LINE_STRIP, block->getVertexCount(), GL_UNSIGNED_INT, nullptr);
}

void BlockRenderer::update()
{
	block->bind();
	Renderer::update();
}

void BlockRenderer::setModelUniform(Vector3 position)
{
	glm::mat4 model;
	model = glm::translate(model, position);
	GLuint modelLoc = glGetUniformLocation(shader->Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void BlockRenderer::setColorUniform(Vector4 color)
{
		//fill color uniform
	GLuint vertexColorLocation = glGetUniformLocation(shader->Program, "ourColor");
	glUniform4f(vertexColorLocation, color.x, color.y, color.z, color.w);
}
