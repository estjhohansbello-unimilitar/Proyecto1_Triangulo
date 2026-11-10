#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "ModelLoader.h"

class Model
{
public:

    std::vector<std::unique_ptr<Mesh>> meshes;

    // Caja envolvente del modelo (en las unidades originales del glTF).
    // Se calcula en LoadMesh y sirve para escalar y centrar el modelo.
    glm::vec3 boundsMin = glm::vec3(1e30f);
    glm::vec3 boundsMax = glm::vec3(-1e30f);

    Model(const tinygltf::Model& gltfModel);

    void LoadMesh(const tinygltf::Model& gltfModel);

    void Draw();

};

#endif