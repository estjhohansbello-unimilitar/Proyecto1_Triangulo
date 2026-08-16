#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
// Guarda las instrucciones que procesan cada vértice de un objeto 3D
//(como cambiar posiciones o calcular coordenadas).
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//guarda el código fuente en texto (GLSL) de un fragment shader en C o C++ 
// para definir el aspecto final de cada píxel en la pantalla.
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";


int main ()
{
	glfwInit();
	// Your code here

// Le indicamos a GLFW qué versión de OpenGL queremos utilizar.
// En este caso: OpenGL 3.3.
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);


	// Le indicamos que queremos utilizar el perfil "Core" de OpenGL.
	// Core Profile utiliza las funciones modernas de OpenGL
	// y elimina muchas funciones antiguas.
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// se añade los vertices para un triangulo equilatero
	Glfloat vertices [] = 
	{
	-0.5f, -0.5f * float(sqrt(3))/3, 0.0f
	 0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f
	 0.0f, 0.5f * float(sqrt(3))*2 / 3, 0.0f
	}

	// Crea una ventana de 800x600 píxeles.
	// "LearnOpenGL" será el título de la ventana.
	//
	// Los dos últimos NULL indican que no estamos usando:
	// - un monitor específico (pantalla completa)
	// - una ventana existente de la cual compartir recursos

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Hace que el contexto de OpenGL de nuestra ventana
// sea el contexto actual.
//
// En otras palabras: le decimos a OpenGL
// "vamos a trabajar con esta ventana".

	glfwMakeContextCurrent(window);

	// GLAD carga las funciones de OpenGL que necesitamos.
	// Esto es necesario porque muchas funciones de OpenGL
	// no están disponibles directamente en Windows.
	gladLoadGL();

	// Define el área de la ventana donde OpenGL va a dibujar.
//
// 0, 0 -> posición inicial (esquina inferior izquierda)
// 800  -> ancho
// 600  -> alto
	glViewport(0, 0, 800, 600);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


	// Limpia el framebuffer utilizando el color definido
	// anteriormente con glClearColor().
	//
	// GL_COLOR_BUFFER_BIT indica que queremos limpiar
	// el buffer de color.
	glClear(GL_COLOR_BUFFER_BIT);

	// Intercambia los buffers.
//
// OpenGL normalmente utiliza un buffer que se está mostrando
// y otro en el que se está dibujando.
//
// Esta función muestra en pantalla lo que acabamos de dibujar.
	glfwSwapBuffers(window);	

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
	}
	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}