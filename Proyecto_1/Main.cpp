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


/*
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
{ //          COORDINATES               /   COLORS (negro)   //

	// --- Oreja izquierda ---
-0.55f,  1.00f, 0.0f,   0.0f, 0.0f, 0.0f,
-0.85f,  0.50f, 0.0f,   0.0f, 0.0f, 0.0f,
0.00f,  0.55f, 0.0f,   0.0f, 0.0f, 0.0f,

// --- Oreja derecha ---
0.55f,  1.00f, 0.0f,   0.0f, 0.0f, 0.0f,
0.00f,  0.55f, 0.0f,   0.0f, 0.0f, 0.0f,
0.85f,  0.50f, 0.0f,   0.0f, 0.0f, 0.0f,

 // --- Cabeza (triangulo ancho arriba, angosto abajo) ---
-0.85f,  0.50f, 0.0f,   0.0f, 0.0f, 0.0f,
0.85f,  0.50f, 0.0f,   0.0f, 0.0f, 0.0f,
0.00f, -0.05f, 0.0f,   0.0f, 0.0f, 0.0f,
// --- Cuerpo, mitad izquierda ---
0.00f, -0.05f, 0.0f,   0.0f, 0.0f, 0.0f,
-0.85f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f,
0.00f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f,

// --- Cuerpo, mitad derecha ---
0.00f, -0.05f, 0.0f,   0.0f, 0.0f, 0.0f,
0.00f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f,
0.85f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f,

 // --- Cola, triangulo 1 ---
0.20f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f,
1.30f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f,
1.60f, -0.55f, 0.0f,   0.0f, 0.0f, 0.0f,

 // --- Cola, triangulo 2 ---
0.20f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f,
1.60f, -0.55f, 0.0f,   0.0f, 0.0f, 0.0f,
0.50f, -0.55f, 0.0f,   0.0f, 0.0f, 0.0f,
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
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(800, 800, "Proyecto1_Triangulo", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, 800, 800);


	
	// Generates Shader object using shaders defualt.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");



	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	
	//EBO EBO1(indices, sizeof(indices));

	// Links VBO to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	
	//EBO1.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		//glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		
		glClearColor(0.2f, 0.6f, 0.3f, 1.0f); // verde

		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		
		//glUniform1f(uniID, 0.5f);
		
		glUniform1f(uniID, -0.7f);

		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawArrays(GL_TRIANGLES, 0, 21); // 7 triangulos x 3 vertices
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	
	//EBO1.Delete();
	
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}