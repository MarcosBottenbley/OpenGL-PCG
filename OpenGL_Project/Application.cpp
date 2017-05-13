#include "Application.h"



Application::Application()
{
	display = new Display(WIDTH, HEIGHT);
	camera = new Camera(Vector3(0.0f, 0.0f, 0.0f));
	renderer = new BlockRenderer(camera);
}


Application::~Application()
{
	delete display;
	delete camera;
	delete renderer;
}

Camera * Application::getCamera()
{
	return this->camera;
}

Display * Application::getDisplay()
{
	return this->display;
}

void Application::runMainGameLoop()
{
	//place the cubes in different locations
	const int platformWidth = 50, platformLength = 50;
	glm::vec3 cubePositions[platformWidth * platformLength];
	for (int x = 0; x < platformWidth; x++)
	{
		for (int z = 0; z < platformLength; z++)
		{
			cubePositions[x * platformWidth + z] = glm::vec3((GLfloat)x, (GLfloat)(rand() % 100 + 1), (GLfloat)z);
		}
	}

	while (display->isOpen())
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();

		//render and clear the color buffer
		renderer->clear();

		//activate first shader
		//shader must be activated before uniforms are filled with data
		renderer->update();

		/*Scale first
		Translate second
		Rotate last
		This order keeps openGl from doing weird things
		*/
		for (GLuint i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++)
		{
			renderer->setColorUniform(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
			//fill model uniform
			renderer->setModelUniform(cubePositions[i]);
			//draw triangles
			renderer->draw();
		}

		glBindVertexArray(0);
		glfwSwapBuffers(display->getWindow());
	}
}

void Application::close()
{
	display->close();
	glfwTerminate();
}
