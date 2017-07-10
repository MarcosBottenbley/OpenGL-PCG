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
#include <Windows.h>
#include <vector>

//custom .h files
#include "Shader.h"
#include "Camera.h"
#include "Perlin.h"
#include "Model.h"

//forces opengl to use the dedicated graphics card
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void doMovement();

const GLuint WIDTH = 1920, HEIGHT = 1080;

GLfloat lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;

bool keys[1024];

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(Vector3(0.0f, 0.0f, 10.0f));

// Light attributes
glm::vec3 lightPos(camera.getX(), 50.0f, camera.getZ());

int octaves = 6;
double persistence = 0.2;
double lacunarity = 5.0;
double scale = 3.0;

GLFWwindow* window;

void init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Simulation", glfwGetPrimaryMonitor(), nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to initilize GLFW" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initilize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	std::cout << glGetString(GL_RENDERER) << std::endl;
}

int main()
{

	init();

	Shader cubeShader("Shaders/blockVertexShader.glsl","Shaders/blockFragmentShader.glsl");
	Shader lampShader("Shaders/lampVertexShader.glsl", "Shaders/lampFragmentShader.glsl");

	Model block("Models/block.model");
	Model light("Models/light.model");

	double lastTime = glfwGetTime();
	int frames = 0;

	Matrix4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 255.0f);


	const int chunkSize = 900;
	const int chunkHeight = 70;
	Perlin* p = new Perlin();

	int chunksX = 3;
	int chunksY = 3;

	std::vector<std::vector<Vector3>> terrainChunks;
	std::vector<Matrix4> terrainModels;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/*
	for (int cx = (camera.getX() - (chunkSize + chunksX) / 2); cx < chunksX + (camera.getX() - (chunkSize + chunksX) / 2); cx++)
	{
		for (int cy = (camera.getZ() - (chunkSize + chunksY) / 2); cy < chunksY + (camera.getZ() - (chunkSize + chunksY) / 2); cy++)
		{
			std::vector<Vector3> terrainVectors;
			for (double r = cx; r < chunkSize + cx; r++)
			{
				for (double c = cy; c < chunkSize + cy; c++)
				{
					double getnoise = 0;
					getnoise = p->OctavePerlin(abs(r) / 30, abs(c) / 30, 0, octaves, persistence, lacunarity, scale);
					terrainVectors.push_back(Vector3((GLfloat)r, (GLfloat)getnoise * chunkHeight, (GLfloat)c));
				}
			}
			terrainChunks.push_back(terrainVectors);
		}
	}
	*/

	for (double r = 0; r < chunkSize; r++)
	{
		for (double c = 0; c < chunkSize; c++)
		{
			double getnoise = 0;
			getnoise = p->OctavePerlin(abs(r) / 30, abs(c) / 30, 0, octaves, persistence, lacunarity, scale);
			Matrix4 model;
			model = glm::translate(model, Vector3((GLfloat)r, (GLfloat)getnoise * chunkHeight, (GLfloat)c));
			terrainModels.push_back(model);
		}
	}

	block.bind();
	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, terrainModels.size() * sizeof(terrainModels[0]), terrainModels.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (GLvoid*)(sizeof(Vector4)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (GLvoid*)(2 * sizeof(Vector4)));
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (GLvoid*)(3 * sizeof(Vector4)));
	glEnableVertexAttribArray(5);

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	block.unbind();

	/*Scale first
	Translate seconds
	Rotate last
	This order keeps openGl from doing weird things
	*/
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		double currentTime = glfwGetTime();
		frames++;
		if (currentTime - lastTime >= 1.0)
		{
			std::cout << frames << std::endl;
			frames = 0;
			lastTime += 1.0;
		}

		glfwPollEvents();
		doMovement();

		Vector4 skyColor = Vector4(0.2f, 0.3f, 0.5f, 1.0f);
		//render and clear the color buffer
		glClearColor(skyColor.x, skyColor.y, skyColor.z, skyColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//activate first shader
		//shader must be activated before uniforms are filled with data
		cubeShader.Use();

		Matrix4 view;
		view = camera.getViewMatrix();

		cubeShader.SetUniform("skyColor", skyColor);
		cubeShader.SetUniform("view", view);
		cubeShader.SetUniform("projection", projection);
		cubeShader.SetUniform("objectColor", Vector3(0.2f, 0.8f, 0.31f));
		cubeShader.SetUniform("lightColor", Vector3(1.0f, 1.0f, 1.0f));
		cubeShader.SetUniform("lightPos", Vector3(lightPos.x, lightPos.y, lightPos.z));
		
		block.bind();
		glDrawElementsInstanced(GL_TRIANGLES, block.getIndexCount(), GL_UNSIGNED_INT, 0, chunkSize * chunkSize);
		block.unbind();

		lampShader.Use();

		Matrix4 model;
		model = glm::translate(model, lightPos);
		model = glm::scale(model, Vector3(0.2f)); // Make it a smaller cube

		// Set uniforms
		lampShader.SetUniform("view", view);
		lampShader.SetUniform("projection", projection);
		lampShader.SetUniform("model", model);

		// Draw the light object (using light's vertex attributes)
		light.bind();
		glDrawElements(GL_TRIANGLES, light.getIndexCount(), GL_UNSIGNED_INT, nullptr);
		light.unbind();

		glfwSwapBuffers(window);
	}
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
	//lightPos = Vector3(camera.getX(), 50.0f, camera.getZ());
	bool oct = keys[GLFW_KEY_O];
	bool pers = keys[GLFW_KEY_P];
	bool lac = keys[GLFW_KEY_L];
	bool scl = keys[GLFW_KEY_K];

	if (keys[GLFW_KEY_W])
		camera.moveCamera(FORWARD, deltaTime * 2);
	if (keys[GLFW_KEY_S])
		camera.moveCamera(BACKWARD, deltaTime * 2);
	if (keys[GLFW_KEY_A])
		camera.moveCamera(LEFT, deltaTime * 2);
	if (keys[GLFW_KEY_D])
		camera.moveCamera(RIGHT, deltaTime * 2);
	if (keys[GLFW_KEY_R])
		camera.reset();
	if (keys[GLFW_KEY_UP])
	{
		lightPos += Vector3(0.0f, 1.0f, 0.0f) * 5.0f * deltaTime;
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
		lightPos += Vector3(0.0f, -1.0f, 0.0f) * 5.0f * deltaTime;
		if (oct)
			octaves--;
		if (pers)
			persistence -= 0.1;
		if (lac)
			lacunarity -= 0.1;
		if (scl)
			scale -= 0.5;
	}
	if (keys[GLFW_KEY_LEFT])
		lightPos += Vector3(-1.0f, 0.0f, 0.0f) * 5.0f * deltaTime;
	if (keys[GLFW_KEY_RIGHT])
		lightPos -= Vector3(-1.0f, 0.0f, 0.0f) * 5.0f * deltaTime;
	if (keys[GLFW_KEY_F])
		lightPos += Vector3(0.0f, 0.0f, -1.0f) * 5.0f * deltaTime;
	if (keys[GLFW_KEY_B])
		lightPos -= Vector3(0.0f, 0.0f, -1.0f) * 5.0f * deltaTime;
}
