#include "Camera.h"

Camera::Camera(Vector3 position)
{
	cameraPos = position;
	cameraDir = glm::normalize(cameraPos - cameraTarget);
	cameraRight = glm::normalize(glm::cross(worldUp, cameraDir));
	cameraUp = glm::normalize(glm::cross(cameraDir, cameraRight));
}

Camera::Camera(Vector3 position, GLfloat sensitivity, GLfloat speed)
{
	cameraPos = position;
	cameraDir = glm::normalize(cameraPos - cameraTarget);
	cameraRight = glm::normalize(glm::cross(worldUp, cameraDir));
	cameraUp = glm::normalize(glm::cross(cameraDir, cameraRight));
	this->sensitivity = sensitivity;
	this->speed = speed;
}

void Camera::rotateCamera(GLfloat xoffset, GLfloat yoffset)
{
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 90.0f)
		pitch = 90.0f;
	if (pitch < -90.0f)
		pitch = -90.0f;

	Vector3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}

void Camera::moveCamera(Movement_Direction direction, GLfloat dt)
{
	if (direction == FORWARD)
		cameraPos += cameraFront * speed * dt;
	if (direction == BACKWARD)
		cameraPos -= cameraFront * speed * dt;
	if (direction == LEFT)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed * dt;
	if (direction == RIGHT)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed * dt;
}

Matrix4 Camera::getViewMatrix()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

GLfloat Camera::getX()
{
	return cameraPos.x;
}

GLfloat Camera::getZ()
{
	return cameraPos.z;
}

void Camera::reset()
{
	cameraPos = Vector3(0.0f, 0.0f, 0.0f);
	cameraFront = Vector3(0.0f, 0.0f, -1.0f);
}
