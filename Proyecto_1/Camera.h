#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	glm::vec3 Position;

	// Hacia dónde "mira" la cámara (vector de dirección)
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);

	// Vector "arriba" del mundo
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	int width;
	int height;

	// Qué tan rápido se mueve la cámara por frame
	float speed = 2.0f;

	Camera(int width, int height, glm::vec3 position);

	// Arma la matriz de vista (dónde está la cámara y hacia dónde mira)
	glm::mat4 GetViewMatrix();

	// Arma la matriz de proyección (perspectiva, FOV, planos near/far)
	glm::mat4 GetProjectionMatrix(float FOVdeg, float nearPlane, float farPlane);

	// Lee las flechas del teclado y mueve la cámara
	void Inputs(GLFWwindow* window, float deltaTime);
};

#endif
