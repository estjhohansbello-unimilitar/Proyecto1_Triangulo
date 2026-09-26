#include"Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Orientation, Up);
}

glm::mat4 Camera::GetProjectionMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	return glm::perspective(
		glm::radians(FOVdeg),
		(float)width / (float)height,
		nearPlane,
		farPlane
	);
}

void Camera::Inputs(GLFWwindow* window, float deltaTime)
{
	float velocity = speed * deltaTime;

	// Vector lateral de la cámara (perpendicular a hacia dónde mira y al "arriba")
	glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));

	// Flecha ARRIBA: avanzar
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		Position += velocity * Orientation;
	}

	// Flecha ABAJO: retroceder
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		Position -= velocity * Orientation;
	}

	// Flecha IZQUIERDA: moverse hacia la izquierda (strafe)
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		Position -= velocity * right;
	}

	// Flecha DERECHA: moverse hacia la derecha (strafe)
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		Position += velocity * right;
	}
}
