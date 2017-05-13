#pragma once
#include "Renderer.h"
#include "Model.h"

class BlockRenderer :
	public Renderer
{
public:
	BlockRenderer(Camera* camera);
	~BlockRenderer();

	void draw();
	void update();

	void setModelUniform(Vector3 position);
	void setColorUniform(Vector4 color);
private:
	Model* block;
};

