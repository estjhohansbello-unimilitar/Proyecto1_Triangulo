#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

// Vertices del gato
// Coordenadas / Color
GLfloat vertices[] =
{
    // --- Oreja izquierda ---
    -0.55f,  1.00f, 0.0f,   1.0f, 0.0f, 0.0f, // v0
    -0.85f,  0.50f, 0.0f,   1.0f, 0.0f, 0.0f, // v1
     0.00f,  0.00f, 0.0f,   1.0f, 0.0f, 0.0f, // v2

     // --- Oreja derecha ---
      0.55f,  1.00f, 0.0f,   0.0f, 0.0f, 1.0f, // v3
      0.00f,  0.00f, 0.0f,   0.0f, 0.0f, 1.0f, // v4
      0.85f,  0.50f, 0.0f,   0.0f, 0.0f, 1.0f, // v5

      // --- Cabeza ---
      -0.85f,  0.50f, 0.0f,   1.0f, 0.0f, 1.0f, // v6
       0.85f,  0.50f, 0.0f,   0.0f, 0.0f, 0.0f, // v7
       0.00f, -0.05f, 0.0f,   1.0f, 0.0f, 1.0f, // v8

       // --- Cuerpo, mitad izquierda ---
        0.00f, -0.05f, 0.0f,   1.0f, 0.0f, 0.0f, // v9
       -0.85f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f, // v10
        0.00f, -0.90f, 0.0f,   0.0f, 0.0f, 1.0f, // v11

        // --- Cuerpo, mitad derecha ---
         0.00f, -0.05f, 0.0f,   0.0f, 0.0f, 1.0f, // v12
         0.00f, -0.90f, 0.0f,   0.0f, 0.0f, 0.0f, // v13
         0.85f, -0.90f, 0.0f,   1.0f, 0.0f, 0.0f, // v14
};


int main()
{
    // =========================================================
    // INICIALIZAR GLFW
    // =========================================================

    glfwInit();

    // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Perfil CORE
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Configurar buffer de profundidad de 24 bits
    glfwWindowHint(GLFW_DEPTH_BITS, 24);


    // =========================================================
    // CREAR VENTANA
    // =========================================================

    GLFWwindow* window = glfwCreateWindow(
        800,
        800,
        "Proyecto1_Triangulo",
        NULL,
        NULL
    );

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Hacer que la ventana sea el contexto actual
    glfwMakeContextCurrent(window);


    // =========================================================
    // CARGAR GLAD
    // =========================================================

    gladLoadGL();

    // Configurar viewport
    glViewport(0, 0, 800, 800);


    // =========================================================
    // CONFIGURAR BUFFER DE PROFUNDIDAD
    // =========================================================

    glEnable(GL_DEPTH_TEST);

    // El fragmento más cercano a la cámara gana
    glDepthFunc(GL_LESS);


    // =========================================================
    // CREAR SHADER
    // =========================================================

    Shader shaderProgram("default.vert", "default.frag");


    // =========================================================
    // CREAR VAO
    // =========================================================

    VAO VAO1;
    VAO1.Bind();


    // =========================================================
    // CREAR VBO
    // =========================================================

    VBO VBO1(vertices, sizeof(vertices));


    // =========================================================
    // CONFIGURAR ATRIBUTOS
    // =========================================================

    // Posición
    VAO1.LinkAttrib(
        VBO1,
        0,
        3,
        GL_FLOAT,
        6 * sizeof(float),
        (void*)0
    );

    // Color
    VAO1.LinkAttrib(
        VBO1,
        1,
        3,
        GL_FLOAT,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );


    // Desenlazar
    VAO1.Unbind();
    VBO1.Unbind();


    // =========================================================
    // VARIABLE UNIFORME
    // =========================================================

    // Buscar la variable uMVP dentro del shader
    GLuint uniMVP = glGetUniformLocation(
        shaderProgram.ID,
        "uMVP"
    );


    // =========================================================
    // RENDER LOOP
    // =========================================================

    while (!glfwWindowShouldClose(window))
    {
        // -----------------------------------------------------
        // COLOR DEL FONDO
        // -----------------------------------------------------

        glClearColor(
            0.2f,
            0.6f,
            0.3f,
            1.0f
        );


        // -----------------------------------------------------
        // LIMPIAR COLOR Y PROFUNDIDAD
        // -----------------------------------------------------

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // -----------------------------------------------------
        // ACTIVAR SHADER
        // -----------------------------------------------------

        shaderProgram.Activate();


        // =====================================================
        // MATRIZ MODELO
        // =====================================================

        glm::mat4 model = glm::mat4(1.0f);


        // =====================================================
        // MATRIZ DE VISTA
        // =====================================================

        glm::mat4 view = glm::lookAt(

            // Posición de la cámara
            glm::vec3(
                0.0f,
                0.0f,
                3.0f
            ),

            // Punto al que mira
            glm::vec3(
                0.0f,
                0.0f,
                0.0f
            ),

            // Dirección hacia arriba
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            )
        );


        // =====================================================
        // MATRIZ DE PROYECCIÓN
        // =====================================================

        glm::mat4 projection = glm::perspective(

            // Campo de visión
            glm::radians(45.0f),

            // Relación ancho / alto
            800.0f / 800.0f,

            // Distancia mínima
            0.1f,

            // Distancia máxima
            100.0f
        );


        // =====================================================
        // MATRIZ MVP
        // =====================================================

        glm::mat4 mvp =
            projection *
            view *
            model;


        // =====================================================
        // ACTUALIZAR VARIABLE UNIFORME
        // =====================================================

        glUniformMatrix4fv(
            uniMVP,
            1,
            GL_FALSE,
            glm::value_ptr(mvp)
        );


        // =====================================================
        // DIBUJAR GATO
        // =====================================================

        VAO1.Bind();

        glDrawArrays(
            GL_TRIANGLES,
            0,
            15
        );


        // =====================================================
        // INTERCAMBIAR BUFFERS
        // =====================================================

        glfwSwapBuffers(window);


        // Procesar eventos
        glfwPollEvents();
    }


    // =========================================================
    // LIBERAR RECURSOS
    // =========================================================

    VAO1.Delete();
    VBO1.Delete();

    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}