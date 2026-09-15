#include <iostream>
#include <cmath> // para las funciones seno y coseno

#include <glad/glad.h> // para cargar las funciones de OpenGL
#include <GLFW/glfw3.h> // para crear la ventana y el contexto de OpenGL

#include <glm/glm.hpp> // permite trabajar con vectores y matrices
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h" 
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"


// ------------------------------------------------------------
// TRIÁNGULO
// Cada vértice tiene:
// posición X,Y,Z + color R,G,B
// ------------------------------------------------------------

GLfloat vertices[] =
{
    // Triángulo 1
    // Posición                 // Color
    -0.5f, -0.5f, 0.0f,        1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,        0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f,        0.0f, 0.0f, 1.0f
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
        "Proyecto OpenGL - 3 Triangulos 3D",
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
    // VAO
    // --------------------------------------------------------

    VAO VAO1;

    VAO1.Bind();


    // --------------------------------------------------------
    // VBO
    // --------------------------------------------------------

    VBO VBO1(
        vertices,
        sizeof(vertices)
    );


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


    VAO1.Unbind();
    VBO1.Unbind();


    // --------------------------------------------------------
    // UNIFORM
    // --------------------------------------------------------

    GLuint uniMVP = glGetUniformLocation(
        shaderProgram.ID,
        "uMVP"
    );


    // --------------------------------------------------------
    // Render loop 
    // --------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // ----------------------------------------------------
        // LIMPIAR PANTALLA
        // ----------------------------------------------------

        glClearColor(
            0.2f,
            0.6f,
            0.3f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // Activar shader
        shaderProgram.Activate();


        // ----------------------------------------------------
        // TIEMPO
        // ----------------------------------------------------

        float tiempo = glfwGetTime();


        // ----------------------------------------------------
        // CÁMARA GIRATORIA
        // ----------------------------------------------------

		float radio = 5.0f; // Radio de la órbita de la cámara

        float cameraX = 
			sin(tiempo * 0.5f) * radio; // Velocidad de rotación de la cámara

        float cameraZ =
			cos(tiempo * 0.5f) * radio; // Velocidad de rotación de la cámara


        glm::mat4 view = glm::lookAt( 

            // Posición de la cámara
            glm::vec3(
                cameraX,
                1.5f,
                cameraZ
            ),

            // Punto al que mira
            glm::vec3(
                0.0f,
                0.0f,
                0.0f
            ),

            // Arriba
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            )
        );


        // ----------------------------------------------------
        // PROYECCIÓN
        // ----------------------------------------------------

        glm::mat4 projection =
            glm::perspective(

                glm::radians(45.0f),

                800.0f / 800.0f,

                0.1f,

                100.0f
            );


        // ----------------------------------------------------
        // TRIÁNGULO 1
        // ----------------------------------------------------

        glm::mat4 model1 =
            glm::mat4(1.0f);


        // Lo colocamos adelante
        model1 =
            glm::translate(
                model1,
                glm::vec3(
                    0.0f,
                    0.0f,
                    0.0f
                )
            );


        glm::mat4 mvp1 =
            projection *
            view *
            model1;


        glUniformMatrix4fv(
            uniMVP,
            1,
            GL_FALSE,
            glm::value_ptr(mvp1)
        );


        VAO1.Bind();


        glDrawArrays(
            GL_TRIANGLES,
            0,
            3
        );


        // ----------------------------------------------------
        // TRIÁNGULO 2
        // ----------------------------------------------------

        glm::mat4 model2 =
            glm::mat4(1.0f);


        // Lo colocamos detrás
        model2 =
            glm::translate(
                model2,
                glm::vec3(
                    0.0f,
                    0.0f,
                    -1.5f
                )
            );


        glm::mat4 mvp2 =
            projection *
            view *
            model2;


        glUniformMatrix4fv(
            uniMVP,
            1,
            GL_FALSE,
            glm::value_ptr(mvp2)
        );


        glDrawArrays(
            GL_TRIANGLES,
            0,
            3
        );


        // ----------------------------------------------------
        // TRIÁNGULO 3
        // ----------------------------------------------------

        glm::mat4 model3 =
            glm::mat4(1.0f);


        // Más atrás
        model3 =
            glm::translate(
                model3,
                glm::vec3(
                    0.0f,
                    0.0f,
                    -3.0f
                )
            );

		// Más pequeño
        glm::mat4 mvp3 =
            projection *
            view *
            model3;


        glUniformMatrix4fv(
            uniMVP,
            1,
            GL_FALSE,
            glm::value_ptr(mvp3)
        );


        glDrawArrays(
            GL_TRIANGLES,
            0,
            3
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

    shaderProgram.Delete();


    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}