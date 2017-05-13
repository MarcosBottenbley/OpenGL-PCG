#pragma once

#define GLEW_STATIC
#include <GL\glew.h>

//GLFW
#include <GLFW\glfw3.h>

#include <iostream>

class Display
{
public:
	Display(GLuint width, GLuint height);
	bool isOpen();
	void close();
	GLFWwindow* getWindow() const;
	GLuint getWidth() const;
	GLuint getHeight() const;

private:
	GLFWwindow* window;
	GLuint width;
	GLuint height;
};

