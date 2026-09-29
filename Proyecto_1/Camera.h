#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

//Up es el vector que define la dirección "arriba" de la cámara, que es necesaria para calcular la matriz de vista correctamente.
//Yaw significa "cabeceo" y pitch significa "inclinación". Son los ángulos que definen la orientación de la cámara en 3D.
class Camera
{
public:
	// Posición de la cámara en el mundo
	// La camara necesita saber dónde está para poder calcular la matriz de vista
	glm::vec3 Position;

	// Hacia dónde "mira" la cámara (vector de dirección)
	// Inicialmente mirando hacia el eje -Z (hacia adelante) ya que en OpenGL el eje Z positivo sale de la pantalla 
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);

	// Vector "arriba" del mundo
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	int width;
	int height;

	// Qué tan rápido se mueve la cámara por frame
	float speed = 2.0f;

	// Control de rotación con el mouse

	// Ángulo horizontal (izquierda/derecha) y vertical (arriba/abajo) de la vista
	float Yaw = -90.0f; // Inicialmente mirando hacia el eje -Z, yaw = -90° para que la cámara mire hacia adelante
	float Pitch = 0.0f; // Inicialmente sin inclinación, pitch = 0° para que la cámara mire al horizonte

	// Qué tan sensible es la cámara al movimiento del mouse
	float sensitivity = 0.1f;

	// Última posición conocida del cursor, para calcular cuánto se movió
	double lastX = 0.0;
	double lastY = 0.0;

	// True la primera vez que se lee el mouse, para no "saltar" de golpe
	bool firstMouse = true;

	Camera(int width, int height, glm::vec3 position);

	// Arma la matriz de vista (dónde está la cámara y hacia dónde mira)
	glm::mat4 GetViewMatrix();

	// Arma la matriz de proyección (perspectiva, FOV, planos near/far)
	glm::mat4 GetProjectionMatrix(float FOVdeg, float nearPlane, float farPlane);

	// Lee las flechas/WASD del teclado y mueve la cámara
	void Inputs(GLFWwindow* window, float deltaTime);

	// Lee el movimiento del mouse y rota la cámara (yaw/pitch)
	void MouseInputs(GLFWwindow* window);
};

#endif
