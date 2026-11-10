
#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <memory>
#include <cstddef>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "ModelLoader.h"

// Información básica de los materiales del modelo glTF
struct MaterialData
{
	glm::vec4 baseColor = glm::vec4(1.0f); // color base (RGBA)

    GLuint texture = 0;

    float alphaCutoff = 0.0f;
};

class Model
{
public:
	// unique_ptr significa que el objeto es dueño de la memoria y se libera automáticamente al destruirse.
    // 
    // Mallas OpenGL creadas a partir del archivo glTF
    std::vector<std::unique_ptr<Mesh>> meshes;

    // Materiales del modelo
    std::vector<MaterialData> materials;

    // Límites espaciales del escenario

	// necesarios porque el escenario puede estar rotado y escalado, y los límites se calculan en el espacio local de cada nodo.
    glm::vec3 boundsMin = glm::vec3(1e30f);
    glm::vec3 boundsMax = glm::vec3(-1e30f);

    // Constructor y destructor
    Model(const tinygltf::Model& gltfModel);

    ~Model();

    // Evitar copias del modelo
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    // Carga de materiales y mallas
    void LoadMaterials(const tinygltf::Model& gltfModel);

    void LoadMesh(const tinygltf::Model& gltfModel);

    // Calcula los límites considerando las transformaciones de los nodos
    void CalculateBounds(const tinygltf::Model& gltfModel);

    // Dibuja el escenario glTF
    void Draw(
        GLuint shaderID,
        const tinygltf::Model& gltfModel,
        const glm::mat4& viewProjection,
        const glm::mat4& sceneTransform
    );

private:

    // Relaciona cada malla glTF con sus primitivas OpenGL
    std::vector<std::vector<size_t>> meshPrimitiveMap;

    // Transformación local de un nodo glTF
    glm::mat4 GetNodeTransform(
        const tinygltf::Node& node
    ) const;

    // Recorre los nodos para calcular los límites del escenario
    void CalculateNodeBounds(
        const tinygltf::Model& gltfModel,
        int nodeIndex,
        const glm::mat4& parentTransform
    );

    // Recorre la jerarquía y dibuja cada nodo
    void DrawNode(
        GLuint shaderID,
        const tinygltf::Model& gltfModel,
        int nodeIndex,
        const glm::mat4& parentTransform,
        const glm::mat4& viewProjection,
        const glm::mat4& sceneTransform
    );

    // Texturas OpenGL creadas durante la carga
    std::vector<GLuint> glTextures;

    // Controla qué texturas ya fueron procesadas
    std::vector<char> textureTried;
};

#endif
