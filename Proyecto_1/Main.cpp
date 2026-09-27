#include <iostream>

#include <glad/glad.h> // para cargar las funciones de OpenGL
#include <GLFW/glfw3.h> // para crear la ventana y el contexto de OpenGL

#include <glm/glm.hpp> // permite trabajar con vectores y matrices
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"



// PIRÁMIDE
// Cada vértice tiene:
// posición X,Y,Z  +  color R,G,B  +  coordenada de textura U,V
// Base cuadrada (4 vértices) + punta (1 vértice) = 5 vértices


GLfloat vertices[] =
{ //     POSICIÓN            //      COLOR           //  TEXCOORD
    -0.5f, 0.0f,  0.5f,       0.83f, 0.70f, 0.44f,     0.0f, 0.0f,
    -0.5f, 0.0f, -0.5f,       0.83f, 0.70f, 0.44f,     5.0f, 0.0f,
     0.5f, 0.0f, -0.5f,       0.83f, 0.70f, 0.44f,     0.0f, 0.0f,
     0.5f, 0.0f,  0.5f,       0.83f, 0.70f, 0.44f,     5.0f, 0.0f,
     0.0f, 0.8f,  0.0f,       0.92f, 0.86f, 0.76f,     2.5f, 5.0f
};

// Índices: qué vértices forman cada triángulo de la pirámide
// (2 triángulos para la base + 4 triángulos para las caras laterales)
GLuint indices[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
};




int main()
{
  
    // INICIALIZAR GLFW

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Activar buffer de profundidad
    glfwWindowHint(GLFW_DEPTH_BITS, 24);



    // CREAR VENTANA
   

    GLFWwindow* window = glfwCreateWindow(
        800,
        800,
        "Proyecto OpenGL - Piramide con textura",
        NULL,
        NULL
    );

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;

        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);

   // Oculta el cursor y lo "atrapa" dentro de la ventana, para que se pueda
  // mover infinitamente en cualquier dirección sin salirse (estilo FPS)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // INICIALIZAR GLAD
  

    gladLoadGL();

    glViewport(0, 0, 800, 800);


    
	// DEPTH TEST que es para que OpenGL sepa qué fragmentos dibujar y cuáles descartar según la profundidad (Z)


    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);



     // SHADER
	//Inicializamos el shader con los archivos de vértices y fragmentos

    Shader shaderProgram("default.vert", "default.frag");


    // VAO / VBO / EBO
  

	VAO VAO1; // Creamos un objeto VAO para la pirámide
	VAO1.Bind(); // Vinculamos el VAO para que las siguientes llamadas afecten a este VAO

	VBO VBO1(vertices, sizeof(vertices)); // Creamos un objeto VBO y le pasamos los vértices de la pirámide
	EBO EBO1(indices, sizeof(indices));// Creamos un objeto EBO y le pasamos los índices de la pirámide

    // Posición -> location 0 (3 floats)
	VAO1.LinkAttrib( // Vinculamos el VBO al VAO y le decimos a OpenGL cómo interpretar los datos de los vértices
        VBO1,
        0,
        3,
        GL_FLOAT,
        8 * sizeof(float),
        (void*)0
    );

    // Color -> location 1 (3 floats)
	VAO1.LinkAttrib( // Vinculamos el VBO al VAO y le decimos a OpenGL cómo interpretar los datos de los vértices
        VBO1,
        1,
        3,
        GL_FLOAT,
        8 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    // Coordenada de textura -> location 2 (2 floats)
    VAO1.LinkAttrib(
        VBO1,
        2,
        2,
        GL_FLOAT,
        8 * sizeof(float),
        (void*)(6 * sizeof(float))
    );

    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();


    
    // TEXTURA
   

	Texture pyramidTex( // Creamos un objeto de textura y le pasamos la ruta de la imagen
        "texture.png",
        GL_TEXTURE_2D,
        GL_TEXTURE0,
        GL_RGBA,
        GL_UNSIGNED_BYTE
    );

	pyramidTex.texUnit(shaderProgram, "tex0", 0); // Le decimos al shader en qué "unidad de textura" (slot) buscar esta textura

    
    // UNIFORM
  
	// Obtenemos la ubicación de la variable uniforme "uMVP" en el shader
    GLuint uniMVP = glGetUniformLocation( 
        shaderProgram.ID,
        "uMVP"
    );


  
    // CÁMARA
   

    Camera camera(800, 800, glm::vec3(0.0f, 1.0f, 3.0f));

    float lastFrame = 0.0f;


   
    // --------------------------------------------------------
    // Render loop
    // --------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // ----------------------------------------------------
        // DELTA TIME (para que la velocidad no dependa del FPS)
        // ----------------------------------------------------

        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        // ----------------------------------------------------
        // INPUT DE CÁMARA (flechas del teclado)
        // ----------------------------------------------------

        camera.Inputs(window, deltaTime);
        camera.MouseInputs(window);

        // ESC cierra el programa (útil porque el cursor queda oculto/atrapado)
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }


        // ----------------------------------------------------
        // LIMPIAR PANTALLA
        // ----------------------------------------------------

        glClearColor(0.2f, 0.6f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Activar shader
        shaderProgram.Activate();


        // ----------------------------------------------------
        // MATRICES DE VISTA Y PROYECCIÓN (desde la cámara)
        // ----------------------------------------------------

        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection = camera.GetProjectionMatrix(45.0f, 0.1f, 100.0f);


        // ----------------------------------------------------
        // MODELO DE LA PIRÁMIDE (gira sola sobre el eje Y)
        // ----------------------------------------------------

        // Grados por segundo que gira la pirámide
        float rotationSpeed = 50.0f;

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::rotate(
            model,
            glm::radians(rotationSpeed) * currentFrame,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 mvp = projection * view * model;

        glUniformMatrix4fv(
            uniMVP,
            1,
            GL_FALSE,
            glm::value_ptr(mvp)
        );


        // ----------------------------------------------------
        // DIBUJAR LA PIRÁMIDE
        // ----------------------------------------------------

        pyramidTex.Bind();

        VAO1.Bind();

        glDrawElements(
            GL_TRIANGLES,
            sizeof(indices) / sizeof(int),
            GL_UNSIGNED_INT,
            0
        );


        // ----------------------------------------------------
        // TERMINAR FRAME
        // ----------------------------------------------------

        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    // --------------------------------------------------------
    // LIBERAR RECURSOS
    // --------------------------------------------------------

    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    pyramidTex.Delete();
    shaderProgram.Delete();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}