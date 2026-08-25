#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"



// Vertices coordinates
// Vertices coordinates - GATO hecho de triangulos (tipo tangram)
// Nota: como el shader multiplica por "scale" (0.5f), estas coordenadas
// se ven mas grandes en el codigo de lo que realmente ocupan en pantalla.


/* Triangulo 
GLfloat vertices[] =
{ //               COORDINATES                  /     COLORS           //
	-0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower left corner
	 0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower right corner
	 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f, // Upper corner
	-0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner left
	 0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner right
	 0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f  // Inner down
};


*/


GLfloat vertices[] =
{ //          Coordenadas  /      COLOR     /

	/*
	
	    V0
	   /  \
	  /	   \
	V1------V2
	
	*/	
	// --- Oreja izquierda ---
-0.55f,  1.00f, 0.0f,   1.0f, 0.0f, 0.0f, //v0
-0.85f,  0.50f, 0.0f,   1.0f, 0.0f, 0.0f, //v1 mmmmm
0.00f,  0.00f, 0.0f,   1.0f, 0.0f, 0.0f, //v2  ----

// --- Oreja derecha ---
0.55f,  1.00f, 0.0f,   0.0f, 0.0f, 1.0f, //v3
0.00f,  0.00f, 0.0f,   0.0f, 0.0f, 1.0f, //v4 ----
0.85f,  0.50f, 0.0f,   0.0f, 0.0f, 1.0f, //v5

 // --- Cabeza (triangulo ancho arriba, angosto abajo) ---
-0.85f,  0.50f, 0.0f,   1.0f, 0.0f, 1.0f, //v6 mmmmmm 
0.85f,  0.50f, 0.0f,   0.0f, 0.0f, 0.0f,  //v7
0.00f, -0.05f, 0.0f,   1.0f, 0.0f, 1.0f, //v8
// --- Cuerpo, mitad izquierda ---
0.00f, -0.05f, 0.0f,   1.0f, 0.0f, 0.0f, //v9
-0.85f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f, //v10
0.00f, -0.90f, 0.0f,   0.0f, 0.0f, 1.0f, //v11

// --- Cuerpo, mitad derecha ---
0.00f, -0.05f, 0.0f,   0.0f, 0.0f, 1.0f, //v12
0.00f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f, //v13	
0.85f, -0.90f, 0.0f,   1.0f, 0.0f, 0.0f, //v14

/* // --- Cola, triangulo 1 ---
0.20f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f, //v15
1.30f, -0.90f, 0.0f,   1.0f, 1.0f, 0.0f, //v16
1.60f, -0.55f, 0.0f,   0.0f, 0.0f, 0.0f, //v17

 // --- Cola, triangulo 2 ---
0.20f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f, //v18
1.60f, -0.55f, 0.0f,   0.0f, 0.0f, 0.0f, //v19
0.50f, -0.55f, 0.0f,   0.0f, 1.0f, 1.0f,//v20
*/
};


// Indices for vertices order
/*GLuint indices[] =
{
	0, 3, 5, // Lower left triangle
	3, 2, 4, // Lower right triangle
	5, 4, 1 // Upper triangle
};
*/

int main()
{
	// Inicializar GLFW
	glfwInit();

	// Indicarle a GLFW qué versión de OpenGL estamos utilizando
	// En este caso estamos utilizando OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Indicarle a GLFW que estamos utilizando el perfil CORE
	// Esto significa que solamente utilizaremos las funciones modernas
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Crear un objeto GLFWwindow de 800 por 800 píxeles, con el nombre "Proyecto1_Triangulo"
	GLFWwindow* window = glfwCreateWindow(800, 800, "Proyecto1_Triangulo", NULL, NULL);
	// Comprobar si la ventana no pudo crearse
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introducir la ventana en el contexto actual
	glfwMakeContextCurrent(window);

	// Cargar GLAD para que configure OpenGL
	gladLoadGL();
	// Especificar el viewport de OpenGL dentro de la ventana
	// En este caso el viewport va desde x = 0, y = 0, hasta x = 800, y = 800
	glViewport(0, 0, 800, 800);



	// Generar el objeto Shader utilizando los shaders default.vert y default.frag
	Shader shaderProgram("default.vert", "default.frag");



	// Generar el Vertex Array Object y enlazarlo
	VAO VAO1;
	VAO1.Bind();

	// Generar el Vertex Buffer Object y enlazarlo con los vértices
	VBO VBO1(vertices, sizeof(vertices));
	// Generar el Element Buffer Object y enlazarlo con los índices

	//EBO EBO1(indices, sizeof(indices));

	// Enlazar el VBO con el VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// Desenlazar todos los objetos para evitar modificarlos accidentalmente
	VAO1.Unbind();
	VBO1.Unbind();

	//EBO1.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Bucle principal
	while (!glfwWindowShouldClose(window))
	{
		// Especificar el color del fondo
		//glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

		glClearColor(0.2f, 0.6f, 0.3f, 1.0f); // verde

		// Limpiar el búfer trasero y asignarle el nuevo color
		glClear(GL_COLOR_BUFFER_BIT);
		// Indicarle a OpenGL qué programa de shaders queremos utilizar
		shaderProgram.Activate();

		//glUniform1f(uniID, 0.5f);

		glUniform1f(uniID, -0.7f);

		// Enlazar el VAO para que OpenGL sepa cuál utilizar
		VAO1.Bind();
		// Dibujar primitivas: número de índices, tipo de dato de los índices, índice de los índices
		glDrawArrays(GL_TRIANGLES, 0, 21); // 7 triángulos x 3 vértices
		// Intercambiar el búfer trasero con el búfer frontal
		glfwSwapBuffers(window);
		// Gestionar todos los eventos de GLFW
		glfwPollEvents();
	}



	// Eliminar todos los objetos que hemos creado
	VAO1.Delete();
	VBO1.Delete();

	//EBO1.Delete();

	shaderProgram.Delete();
	// Eliminar la ventana antes de finalizar el programa
	glfwDestroyWindow(window);
	// Finalizar GLFW antes de terminar el programa
	glfwTerminate();
	return 0;
}