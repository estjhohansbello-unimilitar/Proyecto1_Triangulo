#include <iostream>
#include <algorithm> // std::max
#include <memory>    // std::unique_ptr

#include <glad/glad.h> // para cargar las funciones de OpenGL
#include <GLFW/glfw3.h> // para crear la ventana y el contexto de OpenGL

#include <glm/glm.hpp> // permite trabajar con vectores y matrices
#include <glm/gtc/matrix_transform.hpp> // permite crear matrices de transformación (traslación, rotación, escalado, proyección) 
#include <glm/gtc/type_ptr.hpp> // permite convertir matrices y vectores a punteros de tipo float

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"

#include "Model.h"
//gltf 
#include "ModelLoader.h"


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
     // Punta de la pirámide
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
    // CARGAR EL ARCHIVO GLB (solo CPU, no necesita contexto OpenGL)
    tinygltf::Model gltfModel;

    if (!LoadGLTF("Models/rubber_duck.glb", gltfModel))
    {
        return -1;
    }

    std::cout << "Cantidad de meshes: "
        << gltfModel.meshes.size() << std::endl;

    std::cout << "Cantidad de materiales: "
        << gltfModel.materials.size() << std::endl;

    std::cout << "Cantidad de nodos: "
        << gltfModel.nodes.size() << std::endl;

    // INICIALIZAR GLFW

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //hace que se use la versión 3.3 de OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //hace que se use la versión 3.3 de OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //hace que se use el perfil core de OpenGL (sin funciones obsoletas)

    // Activar buffer de profundidad
    glfwWindowHint(GLFW_DEPTH_BITS, 24);



    // CREAR VENTANA


    GLFWwindow* window = glfwCreateWindow( // Creamos la ventana de 800x800 píxeles
        800,
        800,
        "Proyecto OpenGL - Piramide con textura",
        NULL,
        NULL
    );

    if (window == NULL) // Si no se pudo crear la ventana, mostramos un mensaje de error y terminamos el programa
    {
        std::cout << "Failed to create GLFW window" << std::endl;

        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window); // Hacemos que el contexto de OpenGL sea el de la ventana que acabamos de crear

    // Oculta el cursor y lo "atrapa" dentro de la ventana, para que se pueda
    // mover infinitamente en cualquier dirección sin salirse (estilo FPS)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // INICIALIZAR GLAD

    gladLoadGL(); // Cargamos todas las funciones de OpenGL con GLAD
    // glad hace que podamos usar funciones de OpenGL como glGenBuffers, glBindBuffer, glBufferData que son para crear y manejar buffers de vértices, texturas, shaders, etc.
    glViewport(0, 0, 800, 800);



    // DEPTH TEST que es para que OpenGL sepa qué fragmentos dibujar y cuáles descartar según la profundidad (Z)

    glEnable(GL_DEPTH_TEST); // Habilitamos el test de profundidad para que OpenGL sepa qué fragmentos dibujar y cuáles descartar según la profundidad (Z)

    glDepthFunc(GL_LESS); // Un fragmento se dibuja si su profundidad es menor que la del que ya está en el framebuffer

    // CREAR MODELO 3D
    // Usamos unique_ptr para poder destruirlo explícitamente ANTES de
    // glfwTerminate(): las mallas llaman a glDelete* en su destructor y
    // eso solo es válido mientras el contexto OpenGL siga vivo.
    std::unique_ptr<Model> duck = std::make_unique<Model>(gltfModel);

    // SHADER
    //Inicializamos el shader con los archivos de vértices y fragmentos

    Shader shaderProgram("default.vert", "default.frag");
    Shader modelShader("model.vert", "model.frag");

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
    VAO1.LinkAttrib(
        VBO1,
        1,
        3,
        GL_FLOAT,
        8 * sizeof(float), // Cada vértice tiene 8 floats: 3 de posición, 3 de color y 2 de coordenadas de textura
        (void*)(3 * sizeof(float))
    );

    // Coordenada de textura -> location 2 (2 floats)
    VAO1.LinkAttrib(
        VBO1, // VBO de la pirámide
        2,
        2,
        GL_FLOAT,
        8 * sizeof(float),
        (void*)(6 * sizeof(float)) // La coordenada de textura empieza después de los 6 primeros floats (3 de posición + 3 de color)
    );

    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();



    // TEXTURA


    Texture pyramidTex( // Creamos un objeto de textura y le pasamos la ruta de la imagen
        "calei2.jpg", // Ruta de la imagen
        GL_TEXTURE_2D, // Tipo de textura (2D)
        GL_TEXTURE0, // Slot de textura (0)
        GL_RGBA, // Formato de la textura (RGBA)
        GL_UNSIGNED_BYTE //Especifica el tipo de datos de la imagen (unsigned byte)
    );

    pyramidTex.texUnit(shaderProgram, "tex0", 0); // Le decimos al shader en qué "unidad de textura" (slot) buscar esta textura


    // UNIFORMS

    // Ubicación de "uMVP" en el shader de la pirámide
    GLint uniMVP = glGetUniformLocation(
        shaderProgram.ID,
        "uMVP"
    );
    GLint duckModelUniform = glGetUniformLocation(modelShader.ID, "uModel");
    // Ubicación de "uMVP" en el shader del pato (se consulta una sola vez)
    GLint duckMVPUniform = glGetUniformLocation(
        modelShader.ID,
        "uMVP"
    );


    // CÁMARA


    Camera camera(800, 800, glm::vec3(0.0f, 1.0f, 3.0f)); // Cámara en (0, 1, 3) con ventana de 800x800

    float lastFrame = 0.0f; // Tiempo del último frame, para calcular el delta time


    // MATRIZ DEL PATO (se calcula una sola vez, no cambia entre frames)
    // Normalizamos el modelo: lo centramos en su origen y lo escalamos
    // para que su lado más largo mida 1 unidad, sin importar en qué
    // unidades venga el archivo glTF.

    glm::vec3 size = duck->boundsMax - duck->boundsMin;
    glm::vec3 center = (duck->boundsMin + duck->boundsMax) * 0.5f;
    float maxSide = std::max(size.x, std::max(size.y, size.z));

    glm::mat4 duckModel = glm::mat4(1.0f);
    duckModel = glm::translate(duckModel, glm::vec3(1.5f, 0.0f, 0.0f)); // dónde lo quieres en el mundo
    duckModel = glm::scale(duckModel, glm::vec3(1.0f / maxSide));       // ahora mide ~1 unidad
    duckModel = glm::translate(duckModel, -center);                     // centra el modelo en su origen


    // Render loop

    while (!glfwWindowShouldClose(window))
    {

        // DELTA TIME (para que la velocidad no dependa del FPS)
        float currentFrame = (float)glfwGetTime(); // Tiempo actual en segundos desde que se inició el programa
        float deltaTime = currentFrame - lastFrame;  // Tiempo transcurrido desde el último frame
        lastFrame = currentFrame;


        // INPUT DE CÁMARA

        camera.Inputs(window, deltaTime); // Lee las flechas/WASD del teclado y mueve la cámara
        camera.MouseInputs(window); // Lee el movimiento del mouse y rota la cámara (yaw/pitch)

        // ESC cierra el programa (útil porque el cursor queda oculto/atrapado)
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }



        // LIMPIAR PANTALLA

        glClearColor(0.0f, 0.00f, 0.0f, 0.1f); // color de fondo negro

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpiamos el buffer de color y el de profundidad


        // MATRICES DE VISTA Y PROYECCIÓN (desde la cámara)

        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection = camera.GetProjectionMatrix(45.0f, 0.1f, 100.0f);


        // MODELO DE LA PIRÁMIDE (gira sola sobre el eje Y)

        // Grados por segundo que gira la pirámide 
        float rotationSpeed = 120.0f;

        // Matriz identidad: punto de partida neutral
        glm::mat4 pyramidModel = glm::mat4(1.0f);

        // Rota la pirámide sobre el eje Y (como un trompo). Se recalcula
        // desde cero cada frame con el tiempo total, así el ángulo es exacto.
        pyramidModel = glm::rotate(
            pyramidModel,
            glm::radians(rotationSpeed) * currentFrame, // ángulo actual en radianes
            glm::vec3(0.0f, 1.0f, 0.0f) // eje de rotación: Y (vertical)
        );

        glm::mat4 mvp = projection * view * pyramidModel; // Matriz final: proyección, vista y modelo


        // DIBUJAR LA PIRÁMIDE

        shaderProgram.Activate();

        glUniformMatrix4fv( // Le pasamos la matriz final al shader
            uniMVP,
            1,
            GL_FALSE,
            glm::value_ptr(mvp)
        );

        pyramidTex.Bind(); // Activamos la textura para que se use al dibujar la pirámide

        VAO1.Bind(); // Vinculamos el VAO de la pirámide

        glDrawElements( // Dibujamos la pirámide usando los índices del EBO
            GL_TRIANGLES,
            sizeof(indices) / sizeof(int),
            GL_UNSIGNED_INT,
            0
        );

        // DIBUJAR EL PATO

        modelShader.Activate();

        // Calculamos la matriz MVP del pato
        glm::mat4 duckMVP = projection * view * duckModel;

        // Enviamos la matriz al shader
        glUniformMatrix4fv(
            duckMVPUniform,
            1,
            GL_FALSE,
            glm::value_ptr(duckMVP)
        );
        glUniformMatrix4fv(
            duckModelUniform,
            1,
            GL_FALSE,
            glm::value_ptr(duckModel)
        );

        // Dibujamos todas las mallas del modelo
        duck->Draw();


        // TERMINAR FRAME

        glfwSwapBuffers(window); // Intercambia el buffer de color con el de pantalla

        glfwPollEvents(); // Procesa eventos de entrada (teclado, ratón, etc.)
    }



    // LIBERAR RECURSOS (siempre ANTES de destruir la ventana y terminar GLFW)

    duck.reset(); // destruye las mallas del pato con el contexto aún vivo

    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    pyramidTex.Delete();
    shaderProgram.Delete();
    modelShader.Delete();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}