#include"Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

glm::mat4 Camera::GetViewMatrix() 
// Aqui se arma la matriz de vista, 
//que es la que le dice a OpenGL dónde está la cámara y hacia dónde mira

//position: Donde esta la camara 
//Hacia que punto esta mirando (position + orientation)
// Hacia donde es up (arriba) para que la camara no quede rotada de lado 
{
	return glm::lookAt(Position, Position + Orientation, Up);
}

//GetProjectionMatrix: 
// Arma la matriz de proyección (perspectiva, FOV, planos near/far) la que simula la perspectiva

glm::mat4 Camera::GetProjectionMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	return glm::perspective( //arma la matriz de proyección en perspectiva 
								//(la que hace que las cosas lejanas se vean más chiquitas)
		glm::radians(FOVdeg), //campo de vision (FIEL OF VIEW) en radianes 
		(float)width / (float)height,
		nearPlane, //limites, que tan cerca
		farPlane //limites que tan lejos 
	);
}

void Camera::Inputs(GLFWwindow* window, float deltaTime)
{
	float velocity = speed * deltaTime; //deltaTime es cuanto tiempo paso desde el frame anterior

	// Vector lateral de la cámara (perpendicular a hacia dónde mira y al "arriba")
	// Vector lateral de la cámara ("derecha"), calculado con el producto cruz
	// entre Orientation y Up (el resultado es siempre perpendicular a ambos).
	// normalize() lo deja con longitud 1, para que al multiplicarlo por
	// "velocity" el desplazamiento sea exacto (ni más ni menos).
		glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));

	// Flecha ARRIBA: avanzar
		// le pregunta a GLFW : "¿está esta tecla presionada ahora mismo?".
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += velocity * Orientation;
	}

	// Flecha ABAJO: retroceder
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position -= velocity * Orientation;
	}

	// Flecha IZQUIERDA: moverse hacia la izquierda (strafe)
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position -= velocity * right;
	}

	// Flecha DERECHA: moverse hacia la derecha (strafe)
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += velocity * right;
	}
}
