#pragma once

#include "Glm_Common.h"
#include "GL\glew.h"

#include "Model.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void prepare();
	void render(Model model);
};

