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
#include "Model.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void doMovement();

const GLuint WIDTH = 1024, HEIGHT = 760;

GLfloat lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;

bool keys[1024];

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(Vector3(0.0f, 0.0f, 10.0f));

// Light attributes
glm::vec3 lightPos(1.2f, 50.0f, 2.0f);

int octaves = 4;
double persistence = 20;
double lacunarity = 3;
double scale = 10.0;

GLFWwindow* window;

void init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
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
}

int main()
{
	init();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);

	Shader cubeShader("Shaders/vertexShader.glsl","Shaders/blockFragmentShader.glsl");
	Shader lampShader("Shaders/vertexShader.glsl", "Shaders/lampFragmentShader.glsl");

	const int platformWidth = 16, platformLength = 16;
	Perlin* p = new Perlin();
	double noise[platformWidth][platformLength];

	for (double x = 0; x < platformWidth; x++)
	{
		for (double y = 0; y < platformLength; y++)
		{
			double getnoise = 0;
			getnoise = p->OctavePerlin(x / 30, y / 30, 0, octaves, persistence, lacunarity, scale);
			noise[(int)x][(int)y] = (int)(getnoise * 16);
		}
	}

	Model block("Models/block.model");
	Model light("Models/light.model");

	double lastTime = glfwGetTime();
	int frames = 0;

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

		//render and clear the color buffer
		glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//activate first shader
		//shader must be activated before uniforms are filled with data
		cubeShader.Use();


		glm::mat4 view;
		view = camera.getViewMatrix();

		glm::mat4 projection;
		projection = glm::perspective(45.0f, (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 100.0f);

		//get the locations of the uniforms
		GLuint modelLoc = glGetUniformLocation(cubeShader.Program, "model");
		GLuint viewLoc = glGetUniformLocation(cubeShader.Program, "view");
		GLuint projectionLoc = glGetUniformLocation(cubeShader.Program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		GLint objectColorLoc = glGetUniformLocation(cubeShader.Program, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(cubeShader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(cubeShader.Program, "lightPos");
		glUniform3f(objectColorLoc, 0.2f, 0.8f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		
		block.bind();

		/*Scale first
		  Translate seconds
		  Rotate last
		  This order keeps openGl from doing weird things
		*/
		for (int x = 0; x < platformWidth; x++)
		{
			for (int z = 0; z < platformLength; z++)
			{
				for (int y = 0; y <= noise[x][z]; y++)
				{
					glm::mat4 model;
					model = glm::translate(model, Vector3((GLfloat)x, (GLfloat) y, (GLfloat)z));
					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
					GLuint query;
					glGenQueries(1, &query);
					glBeginConditionalRender(query, GL_QUERY_WAIT);
					glDrawElements(GL_TRIANGLES, block.getVertexCount(), GL_UNSIGNED_INT, nullptr);
					glEndConditionalRender();
				}
			}
		}
		block.unbind();

		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projectionLoc = glGetUniformLocation(lampShader.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glm::mat4 model;
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		light.bind();
		glDrawElements(GL_TRIANGLES, light.getVertexCount(), GL_UNSIGNED_INT, nullptr);
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
