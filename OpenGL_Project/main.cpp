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

	Shader cubeShader("Shaders/vertexShader.glsl","Shaders/blockFragmentShader.glsl");
	Shader lampShader("Shaders/vertexShader.glsl", "Shaders/lampFragmentShader.glsl");

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
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f
	};

	GLfloat normals[] = {
		//back
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,

		//right
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,

		//front
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,

		//left
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,

		//top
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,

		//bottom
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f
	};

	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20
	};

	const int platformWidth = 16, platformLength = 16;
	Perlin* p = new Perlin();
	double noise[platformWidth][platformLength];

	for (double x = 0; x < platformWidth; x++)
	{//Loops to loop trough all the pixels
		for (double y = 0; y < platformLength; y++)
		{
			double getnoise = 0;
			getnoise = p->OctavePerlin(x / 30, y / 30, 0, octaves, persistence, lacunarity, scale);
			noise[(int)x][(int)y] = (int)(getnoise * 16);
		}
	}

	// First, set the container's VAO (and VBO)
	GLuint VBO, VBON, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBON);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, VBON);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the normal vectors
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

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
		{ // If last prinf() was more than 1 sec ago
		  // printf and reset timer
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
		
		glBindVertexArray(VAO);

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
					glDrawElements(GL_TRIANGLES, sizeof(vertices) / sizeof(vertices[0]), GL_UNSIGNED_INT, nullptr);
					glEndConditionalRender();
				}
			}
		}
		glBindVertexArray(0);

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
		glBindVertexArray(lightVAO);
		glDrawElements(GL_TRIANGLE_FAN, sizeof(vertices) / sizeof(vertices[0]), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VBON);
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
