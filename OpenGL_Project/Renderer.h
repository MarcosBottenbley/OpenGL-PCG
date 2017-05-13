#pragma once
#include "Shader.h"
#include "Camera.h"

class Renderer
{
public:
	Renderer(Shader* shader, Camera* camera);
	~Renderer();

	void clear();

private:
	Camera* camera;
protected:
	virtual void update();
	void setUniforms(GLfloat width, GLfloat height);
	Shader* shader;
};