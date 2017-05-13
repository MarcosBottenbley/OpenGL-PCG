#include "Display.h"

Display::Display(GLuint width, GLuint height)
{
	this->width = width;
	this->height = height;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to initilize GLFW" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initilize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	glViewport(0, 0, width, height);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool Display::isOpen()
{
	return (!glfwWindowShouldClose(window));
}

void Display::close()
{
	glfwSetWindowShouldClose(window, GL_TRUE);
}

GLFWwindow * Display::getWindow() const
{
	return window;
}

GLuint Display::getWidth() const
{
	return width;
}

GLuint Display::getHeight() const
{
	return height;
}
