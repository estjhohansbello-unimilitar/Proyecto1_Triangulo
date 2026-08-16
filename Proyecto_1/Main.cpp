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
	// ============================================================
// VERTEX SHADER
// ============================================================

// Crea un objeto de tipo Vertex Shader.
// GL_VERTEX_SHADER indica que este shader se encargará
// de procesar los vértices.
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);


	// Le proporciona el código fuente al Vertex Shader.
	//
	// vertexShader       -> shader al que le damos el código.
	// 1                  -> cantidad de strings que estamos pasando.
	// &vertexShaderSource -> dirección donde está nuestro código fuente.
	// NULL               -> no estamos especificando una longitud manual.
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);


	// Compila el código fuente del Vertex Shader.
	// Convierte el código GLSL en una forma que OpenGL pueda ejecutar.
	glCompileShader(vertexShader);


	// ============================================================
	// FRAGMENT SHADER
	// ============================================================

	// Crea un objeto de tipo Fragment Shader.
	// GL_FRAGMENT_SHADER indica que este shader se encargará
	// de determinar el color de los fragmentos/píxeles.
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);


	// Le proporciona el código fuente al Fragment Shader.
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);


	// Compila el código fuente del Fragment Shader.
	glCompileShader(fragmentShader);


	// ============================================================
	// SHADER PROGRAM
	// ============================================================

	// Crea un Shader Program.
	// Este será el programa que contiene y conecta nuestros
	// Vertex Shader y Fragment Shader.
	GLuint shaderProgram = glCreateProgram();


	// Adjunta el Vertex Shader al Shader Program.
	glAttachShader(shaderProgram, vertexShader);


	// Adjunta el Fragment Shader al Shader Program.
	glAttachShader(shaderProgram, fragmentShader);


	// Enlaza (link) los shaders entre sí.
	//
	// OpenGL comprueba que el Vertex Shader y el Fragment Shader
	// puedan trabajar juntos y crea el programa final.
	glLinkProgram(shaderProgram);


	// ============================================================
	// LIMPIEZA
	// ============================================================

	// Una vez que los shaders fueron enlazados al programa,
	// ya no necesitamos mantener estos objetos por separado.
	//
	// El Shader Program conserva el código necesario,
	// por lo que podemos eliminar los objetos originales.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// El siguiente bloque crea y configura un VAO (Vertex Array Object) y un VBO (Vertex Buffer Object).
// El VBO almacena los datos de los vértices en la memoria de la GPU, mientras que el VAO
// guarda la configuración necesaria para interpretar esos datos. También se establece que
// cada vértice está compuesto por 3 valores de tipo float (coordenadas X, Y, Z) y se habilita
// el atributo de vértice en la posición 0. Finalmente, se desvinculan el VBO y el VAO para
// evitar modificaciones accidentales.

	GLuint VAO,VBO;
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Limpia el framebuffer utilizando el color definido
	// anteriormente con glClearColor().
	//
	// GL_COLOR_BUFFER_BIT indica que queremos limpiar
	// el buffer de color.
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Intercambia los buffers.
//
// OpenGL normalmente utiliza un buffer que se está mostrando
// y otro en el que se está dibujando.
//
// Esta función muestra en pantalla lo que acabamos de dibujar.
	glfwSwapBuffers(window);	

	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		//activamos el shaderProgram para que OpenGL lo use 
		// en el siguiente renderizado
		glUseProgram(shaderProgram);

		// Vincula el VAO que contiene la configuración de los vértices.
		glBindVertexArray(VAO);

		// Dibuja el triángulo utilizando los vértices almacenados en el VBO.
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);

		glfwPollEvents();
		
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}