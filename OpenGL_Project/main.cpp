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
double persistence = 0.25;
double lacunarity = 2.0;
double scale = 1.0;

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

	Shader cubeShader("Shaders/vertexShader.glsl","Shaders/blockFragmentShader.glsl");
	Shader lampShader("Shaders/vertexShader.glsl", "Shaders/lampFragmentShader.glsl");

	Model block("Models/block2.model");
	Model light("Models/light.model");

	double lastTime = glfwGetTime();
	int frames = 0;

	//get the locations of the uniforms
	GLuint modelLoc = glGetUniformLocation(cubeShader.Program, "model");
	GLuint viewLoc = glGetUniformLocation(cubeShader.Program, "view");
	GLuint projectionLoc = glGetUniformLocation(cubeShader.Program, "projection");

	// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
	GLuint lampModelLoc = glGetUniformLocation(lampShader.Program, "model");
	GLuint lampViewLoc = glGetUniformLocation(lampShader.Program, "view");
	GLuint lampProjectionLoc = glGetUniformLocation(lampShader.Program, "projection");

	GLint objectColorLoc = glGetUniformLocation(cubeShader.Program, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(cubeShader.Program, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(cubeShader.Program, "lightPos");

	//for fog
	GLint skyColorLoc = glGetUniformLocation(cubeShader.Program, "skyColor");

	Matrix4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 255.0f);


	const int chunkSize = 30;
	const int chunkHeight = 16;
	Perlin* p = new Perlin();

	int chunksX = 3;
	int chunksY = 3;

	std::vector<std::vector<Vector3>> terrainChunks;

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

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

		for (int cx = (camera.getX() - (chunkSize + chunksX) /2); cx < chunksX + (camera.getX() - (chunkSize + chunksX) / 2); cx++)
		{
			for (int cy = (camera.getZ() - (chunkSize + chunksY) /2); cy < chunksY + (camera.getZ() - (chunkSize + chunksY) / 2); cy++)
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

		Vector4 skyColor = Vector4(0.2f, 0.3f, 0.5f, 1.0f);
		//render and clear the color buffer
		glClearColor(skyColor.x, skyColor.y, skyColor.z, skyColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//activate first shader
		//shader must be activated before uniforms are filled with data
		cubeShader.Use();

		//comment to test git stuff
		glUniform4f(skyColorLoc, skyColor.x, skyColor.y, skyColor.z, skyColor.w);

		Matrix4 view;
		view = camera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		glUniform3f(objectColorLoc, 0.2f, 0.8f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		
		block.bind();

		/*Scale first
		  Translate seconds
		  Rotate last
		  This order keeps openGl from doing weird things*/
		for (int x = 0; x < terrainChunks.size(); x++)
		{
			for (int y = 0; y < terrainChunks[x].size(); y++)
			{
				Matrix4 model;
				model = glm::translate(model, terrainChunks[x][y]);
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				glDrawElements(GL_TRIANGLES, block.getIndexCount(), GL_UNSIGNED_INT, nullptr);
			}
		}
		terrainChunks.clear();
		block.unbind();

		lampShader.Use();
		// Set matrices
		glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(lampProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		Matrix4 model;
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(model));
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
