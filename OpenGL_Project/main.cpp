//GLEW
#define GLEW_STATIC
#include <GL\glew.h>

//GLFW
#include <GLFW\glfw3.h>

//GLM
#include "Glm_Common.h"

//other std libraries
#include <iostream>
#include <math.h>

//custom .h files
#include "Shader.h"
#include "Camera.h"
#include "Perlin.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void doMovement();

const GLuint WIDTH = 1024, HEIGHT = 760;

GLfloat lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;

bool keys[1024];

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(Vector3(0.0f, 0.0f, 10.0f));

int octaves = 4;
double persistence = 0.5;
double lacunarity = 1.5;
double scale = 10.0;

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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	Shader myShader("Shaders/vertexShader.glsl","Shaders/fragmentShader.glsl");

	GLfloat vertices[] = {
		//back
		 0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,

		//right
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,

		//front
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		//left
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,

		//top
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		//bottom
		-0.5f, -0.5f, -0.5f,	//front left - 16
		 0.5f, -0.5f, -0.5f,	//front right - 17
		 0.5f, -0.5f,  0.5f,	//back right - 18
		-0.5f, -0.5f,  0.5f		//back left - 19
	};

	
	GLuint indices[] = {
		//back
		0, 1, 2,
		2, 3, 0,

		//front
		4, 5, 6,
		6, 7, 4,

		//left
		8, 9, 10,
		10, 11, 8,

		//right
		12, 13, 14,
		14, 15, 12,

		//bottom
		16, 17, 18,
		18, 19, 16,

		//top
		20, 21, 22,
		22, 23, 20
	};

	const int platformWidth = 50, platformLength = 50;
	Perlin* p = new Perlin();
	double noise[platformWidth][platformLength];

	GLuint VBO, VAO, EBO;

	//generating VAOs
	glGenVertexArrays(1, &VAO);
	//generating VBOs
	glGenBuffers(1, &VBO);
	//generating EBOs
	glGenBuffers(1, &EBO);

	//=========
	//CUBE
	//=========
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//unbind GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		doMovement();

		//realtime perlin noise
		for (double x = 0; x < platformWidth; x++)
		{//Loops to loop trough all the pixels
			for (double y = 0; y < platformLength; y++)
			{
				double getnoise = 0;
				getnoise = p->OctavePerlin(x / 30, y / 30, 0, octaves, persistence, lacunarity, scale);
				noise[(int)x][(int)y] = (int)(getnoise * 50);
			}
		}

		//render and clear the color buffer
		glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view;
		view = camera.getViewMatrix();

		glm::mat4 projection;
		projection = glm::perspective(45.0f, (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 100.0f);

		//get the locations of the uniforms
		GLuint modelLoc = glGetUniformLocation(myShader.Program, "model");
		GLuint viewLoc = glGetUniformLocation(myShader.Program, "view");
		GLuint projectionLoc = glGetUniformLocation(myShader.Program, "projection");
		GLuint vertexColorLocation = glGetUniformLocation(myShader.Program, "ourColor");

		glBindVertexArray(VAO);
		//activate first shader
		//shader must be activated before uniforms are filled with data
		myShader.Use();

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		/*Scale first
		  Translate seconds
		  Rotate last
		  This order keeps openGl from doing weird things
		*/
		for (int x = 0; x < platformWidth; x++)
		{
			for (int z = 0; z < platformLength; z++)
			{
				for (double y = 0; y <= noise[x][z]; y+= 1.0)
				{
					if (!(x > 0 && x < platformWidth -1 && z > 0 && z < platformLength - 1) || y >= (int) noise[x][z] - 3)
					{
						glm::mat4 model;
						model = glm::translate(model, Vector3((GLfloat)x, (GLfloat)y, (GLfloat)z));
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawElements(GL_TRIANGLE_FAN, sizeof(vertices) / sizeof(vertices[0]), GL_UNSIGNED_INT, nullptr);
						glUniform4f(vertexColorLocation, 0.112f, 0.128f, 0.144f, 1.0f);
						glLineWidth(3.0f);
						glDrawElements(GL_LINE_LOOP, sizeof(vertices) / sizeof(vertices[0]), GL_UNSIGNED_INT, nullptr);
						glUniform4f(vertexColorLocation, 0.2f, 0.6f, 0.2f, 1.0f);
					}
				}
			}
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

	GLfloat xoffset = (GLfloat) (xpos - lastX);
	GLfloat yoffset = (GLfloat) (lastY - ypos);
	lastX = (GLfloat) xpos;
	lastY = (GLfloat) ypos;

	camera.rotateCamera(xoffset, yoffset);
}

void doMovement()
{
	bool oct = keys[GLFW_KEY_O];
	bool pers = keys[GLFW_KEY_P];
	bool lac = keys[GLFW_KEY_L];
	bool scl = keys[GLFW_KEY_K];

	if (keys[GLFW_KEY_W])
		camera.moveCamera(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.moveCamera(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.moveCamera(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.moveCamera(RIGHT, deltaTime);
	if (keys[GLFW_KEY_R])
		camera.reset();
	if (keys[GLFW_KEY_UP])
	{
		if (oct)
			octaves++;
		if (pers)
			persistence += 0.1;
		if (lac)
			lacunarity += 0.1;
		if (scl)
			scale += 0.5;
	}
	else if (keys[GLFW_KEY_DOWN])
	{
		if (oct)
			octaves--;
		if (pers)
			persistence -= 0.1;
		if (lac)
			lacunarity -= 0.1;
		if (scl)
			scale -= 0.5;
	}
}
