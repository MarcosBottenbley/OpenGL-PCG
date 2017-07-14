#pragma once

#include "Glm_Common.h"
#include "GL\glew.h"

#include "Model.h"
#include "Shader.h"

class Renderer
{
public:
	Renderer(Model& model, Shader& shader);
	~Renderer();

	void prepare();
	virtual void render() = 0;
	virtual void getUniformLocations() = 0;
protected:
	Model* model;
	Shader* shader;
private:
	Vector4 skyColor = Vector4(0.2f, 0.3f, 0.5f, 1.0f);
};

