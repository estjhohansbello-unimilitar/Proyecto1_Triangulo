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


// ------------------------------------------------------------
// PIRÁMIDE
// Cada vértice tiene:
// posición X,Y,Z  +  color R,G,B  +  coordenada de textura U,V
// Base cuadrada (4 vértices) + punta (1 vértice) = 5 vértices
// ------------------------------------------------------------

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
    // --------------------------------------------------------
    // INICIALIZAR GLFW
    // --------------------------------------------------------

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Activar buffer de profundidad
    glfwWindowHint(GLFW_DEPTH_BITS, 24);


    // --------------------------------------------------------
    // CREAR VENTANA
    // --------------------------------------------------------

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



    // --------------------------------------------------------
    // INICIALIZAR GLAD
    // --------------------------------------------------------

    gladLoadGL();

    glViewport(0, 0, 800, 800);


    // --------------------------------------------------------
    // DEPTH TEST
    // --------------------------------------------------------

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);


    // --------------------------------------------------------
     // SHADER
     // --------------------------------------------------------

    Shader shaderProgram("default.vert", "default.frag");


    // --------------------------------------------------------
    // VAO / VBO / EBO
    // --------------------------------------------------------

    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    // Posición -> location 0 (3 floats)
    VAO1.LinkAttrib(
        VBO1,
        0,
        3,
        GL_FLOAT,
        8 * sizeof(float),
        (void*)0
    );

    // Color -> location 1 (3 floats)
    VAO1.LinkAttrib(
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


    // --------------------------------------------------------
    // TEXTURA
    // --------------------------------------------------------

    Texture pyramidTex(
        "texture.png",
        GL_TEXTURE_2D,
        GL_TEXTURE0,
        GL_RGBA,
        GL_UNSIGNED_BYTE
    );

    pyramidTex.texUnit(shaderProgram, "tex0", 0);

    // --------------------------------------------------------
    // UNIFORM
    // --------------------------------------------------------

    GLuint uniMVP = glGetUniformLocation(
        shaderProgram.ID,
        "uMVP"
    );


    // --------------------------------------------------------
    // CÁMARA
    // --------------------------------------------------------

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
        // MODELO DE LA PIRÁMIDE (quieta en el origen)
        // ----------------------------------------------------

        glm::mat4 model = glm::mat4(1.0f);

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