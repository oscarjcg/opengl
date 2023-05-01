#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	// Move
	if (keys[GLFW_KEY_W])
	{
		position += front * velocity;
	}

	if (keys[GLFW_KEY_S])
	{
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_D])
	{
		position += right * velocity;
	}

	GLfloat sensibility = 5;
	// Aim
	if (keys[GLFW_KEY_RIGHT])
	{
		yaw += velocity * sensibility;
		update();
	}

	if (keys[GLFW_KEY_LEFT])
	{
		yaw -= velocity * sensibility;
		update();
	}

	if (keys[GLFW_KEY_DOWN])
	{
		pitch -= velocity * sensibility;
		update();
	}

	if (keys[GLFW_KEY_UP])
	{
		pitch += velocity * sensibility;
		update();
	}

	// Shoot
	/*
	if (keys[GLFW_KEY_SPACE])
	{
		printf("GLFW_MOUSE_BUTTON_LEFT \n");
		keysConsumed[GLFW_MOUSE_BUTTON_LEFT] = true;

		actions->Shoot();
	}
	*/
}

void Camera::mouseControl(GameControllerActions* actions, 
	bool* keys, bool* keysConsumed, GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}
	
	
	if (keys[GLFW_MOUSE_BUTTON_LEFT] && !keysConsumed[GLFW_MOUSE_BUTTON_LEFT])
	{
		printf("GLFW_MOUSE_BUTTON_LEFT \n");
		keysConsumed[GLFW_MOUSE_BUTTON_LEFT] = true;

		actions->Shoot();
	}
	

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition()
{
	return position;	
}
glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}


Camera::~Camera()
{
}
