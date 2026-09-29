#include"Camera.h"
#include<cmath>

//yaw significa "cabeceo" y pitch significa "inclinación". Son los ángulos que definen la orientación de la cámara en 3D. 
// Yaw es el ángulo de rotación alrededor del eje vertical (izquierda/derecha), 
// Pitch es el ángulo de rotación alrededor del eje horizontal (arriba/abajo).

//Se utiliza up para definir la dirección "arriba" de la cámara, 
// que es necesaria para calcular la matriz de vista correctamente.

Camera::Camera(int width, int height, glm::vec3 position) // Constructor de la cámara
{
	Camera::width = width; // Inicializa el ancho de la ventana
	Camera::height = height; // Inicializa el alto de la ventana
	Position = position; // Inicializa la posición de la cámara
}
// Arma la matriz de vista (dónde está la cámara y hacia dónde mira)
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Orientation, Up); // La cámara está en "Position", 
	//mirando hacia "Position + Orientation" (hacia adelante), con el vector "Up" como referencia de arriba
}
// Arma la matriz de proyección (perspectiva, FOV, planos near/far)
glm::mat4 Camera::GetProjectionMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	return glm::perspective(
		glm::radians(FOVdeg), // Convertimos los grados a radianes, porque glm::perspective espera radianes
		(float)width / (float)height,
		nearPlane,
		farPlane
	);
}
// Lee el movimiento del mouse y rota la cámara (yaw/pitch)
void Camera::MouseInputs(GLFWwindow* window)
{
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// La primera vez que entramos aquí no hay "posición anterior" del mouse,
	// así que solo la guardamos, sin rotar nada (para evitar un salto brusco).
	if (firstMouse)
	{
		lastX = mouseX; // Guardamos la posición actual del mouse como la última posición conocida
		lastY = mouseY; // Guardamos la posición actual del mouse como la última posición conocida
		firstMouse = false;
	}

	// Cuánto se movió el mouse desde el frame anterior
	float xOffset = (float)(mouseX - lastX);
	// Y invertido: en pantalla, Y crece hacia abajo, pero queremos que
	// mover el mouse hacia arriba incline la cámara hacia arriba.
	float yOffset = (float)(lastY - mouseY);

	lastX = mouseX; // Actualizamos la última posición conocida del mouse
	lastY = mouseY; // Actualizamos la última posición conocida del mouse

	xOffset *= sensitivity; // Aplicamos la sensibilidad al movimiento del mouse
	yOffset *= sensitivity; // Aplicamos la sensibilidad al movimiento del mouse

	Yaw += xOffset; // Actualizamos el ángulo yaw (izquierda/derecha) de la cámara
	Pitch += yOffset; // Actualizamos el ángulo pitch (arriba/abajo) de la cámara

	// Evita que la cámara se voltee de cabeza al mirar demasiado arriba/abajo
	//Esto porque si el pitch es mayor a 89° o menor a -89°, la cámara se voltea y se invierte la orientación, 
	// lo que puede causar un efecto de "giro de cámara" no deseado.

	//Gimbal Lock (o "singularidad en los polos") 
	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	// Convertir los ángulos yaw/pitch en un vector de dirección (Orientation)
	glm::vec3 direction;
	direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)); // coseno de yaw * coseno de pitch
	direction.y = sin(glm::radians(Pitch)); // seno de pitch
	direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch)); // seno de yaw * coseno de pitch

	Orientation = glm::normalize(direction); // Normalizamos para que tenga longitud 1, evitando problemas al movernos
}

void Camera::Inputs(GLFWwindow* window, float deltaTime)
{
	float velocity = speed * deltaTime; // Velocidad de movimiento por frame, 
	//deltaTime es el tiempo que tardó en renderizarse el frame anterior, 
	//así nos aseguramos de que la velocidad sea constante sin importar los FPS.

	// Vector lateral de la cámara (perpendicular a hacia dónde mira y al "arriba")
	glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));

	// Flecha ARRIBA: avanzar
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
