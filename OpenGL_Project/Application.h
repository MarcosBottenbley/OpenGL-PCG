#pragma once

#include "Camera.h"
#include "Display.h"
#include "BlockRenderer.h"

class Application
{
public:
	Application();
	~Application();

	Camera* getCamera();
	Display* getDisplay();
	void runMainGameLoop();
	void close();
private:
	const GLuint WIDTH = 800, HEIGHT = 600;
	Display* display;
	Camera* camera;
	BlockRenderer* renderer;
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;
};

