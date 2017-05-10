//GLEW
#define GLEW_STATIC
#include <GL\glew.h>

//GLFW
#include <GLFW\glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//other std libraries
#include <iostream>

//custom .h files
#include "Shader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void doMovement();

const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDir = glm::normalize(cameraPos - cameraTarget);
glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDir));
glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDir, cameraRight));

GLfloat lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;
GLfloat pitch = 0.0f;
GLfloat yaw = -90.0f;

bool keys[1024];

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to initilize GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initilize GLEW" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);

	Shader myShader("shader.vs","shader.frag");

	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),

		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(-1.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  0.0f,  0.0f),
		glm::vec3(-2.0f,  0.0f,  0.0f),

		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, -2.0f),

		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 2.0f),
		glm::vec3(1.0f, 0.0f, -1.0f),
		glm::vec3(1.0f, 0.0f, -2.0f),
		glm::vec3(2.0f, 0.0f, 1.0f),
		glm::vec3(2.0f, 0.0f, 2.0f),
		glm::vec3(2.0f, 0.0f, -1.0f),
		glm::vec3(2.0f, 0.0f, -2.0f),

		glm::vec3(-1.0f, 0.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 2.0f),
		glm::vec3(-1.0f, 0.0f, -1.0f),
		glm::vec3(-1.0f, 0.0f, -2.0f),
		glm::vec3(-2.0f, 0.0f, 1.0f),
		glm::vec3(-2.0f, 0.0f, 2.0f),
		glm::vec3(-2.0f, 0.0f, -1.0f),
		glm::vec3(-2.0f, 0.0f, -2.0f)
	};

	GLuint VBO, VAO, EBO;

	//generating VAOs
	glGenVertexArrays(1, &VAO);
	//generating VBOs
	glGenBuffers(1, &VBO);
	//generating EBOs
	glGenBuffers(1, &EBO);

	//=========
	//RECTANGLE
	//=========
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//unbind GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		doMovement();

		//render and clear the color buffer
		glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double timeValue = glfwGetTime();
		double redValue = (sin(timeValue)) + 0.5f;
		double greenValue = (cos(timeValue)) + 0.5f;

		glm::mat4 view;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glm::mat4 projection;
		projection = glm::perspective(45.0f, (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 100.0f);

		//get the locations of the uniforms
		GLuint modelLoc = glGetUniformLocation(myShader.Program, "model");
		GLuint viewLoc = glGetUniformLocation(myShader.Program, "view");
		GLuint projectionLoc = glGetUniformLocation(myShader.Program, "projection");

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(VAO);
		//activate first shader
		//shader must be activated before uniforms are filled with data
		myShader.Use();

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		//set color of rectangle
		GLuint vertexColorLocation = glGetUniformLocation(myShader.Program, "ourColor");
		//glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
		glUniform4f(vertexColorLocation, (GLfloat)greenValue, (GLfloat)redValue, 0.0f, 1.0f);

		/*Scale first
		  Translate seconds
		  Rotate last
		  This order keeps openGl from doing weird things
		*/
		for (GLuint i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, (GLfloat)glfwGetTime() * 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glUniform4f(vertexColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
			glLineWidth(3.0f);
			glDrawArrays(GL_LINE_STRIP, 0, 36);
			//glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(vertexColorLocation, (GLfloat)greenValue, 0.0f, (GLfloat)redValue, 1.0f);
		}

		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key >= 0 && key < 1024)
		{
			keys[key] = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key >= 0 && key < 1024)
		{
			keys[key] = false;
		}
	}
}

bool firstMouse = true;
void cursor_position_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (GLfloat) xpos;
		lastY = (GLfloat) ypos;
		firstMouse = false;
	}

	GLfloat xOffset = (GLfloat) (xpos - lastX);
	GLfloat yOffset = (GLfloat) (lastY - ypos);
	lastX = (GLfloat) xpos;
	lastY = (GLfloat) ypos;

	GLfloat sensitivity = 0.05f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += (GLfloat) xOffset;
	pitch += (GLfloat) yOffset;

	if (pitch > 90.0f)
		pitch = 90.0f;
	if (pitch < -90.0f)
		pitch = -90.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}

void doMovement()
{
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraFront * cameraSpeed;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraFront * cameraSpeed;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_R])
		cameraPos = glm::vec3(0.0f, 0.0f, -1.0);
}
