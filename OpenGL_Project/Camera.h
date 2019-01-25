#pragma once

#include <glad\glad.h>

#include "Glm_Common.h"

enum Movement_Direction
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
public:
	Camera(Vector3 position);
	Camera(Vector3 position, GLfloat sensitivity, GLfloat speed);

	void rotateCamera(GLfloat xoffset, GLfloat yoffset);
	void moveCamera(Movement_Direction direction, GLfloat dt);
	Matrix4 getViewMatrix();
	GLfloat getX();
	GLfloat getZ();
	void reset();

private:
	//camera vectors
	Vector3 worldUp      =	Vector3(0.0f, 1.0f,  0.0f);
	Vector3 cameraPos    =	Vector3(0.0f, 0.0f,  0.0f);
	Vector3 cameraFront  =	Vector3(0.0f, 0.0f, -1.0f);
	Vector3 cameraTarget =	Vector3(0.0f, 0.0f,  0.0f);
	Vector3 cameraDir;
	Vector3 cameraRight;
	Vector3 cameraUp;

	GLfloat pitch = 0.0f;
	GLfloat yaw = -90.0f;
	GLfloat sensitivity = 0.05f;
	GLfloat speed = 5.0f;
};

