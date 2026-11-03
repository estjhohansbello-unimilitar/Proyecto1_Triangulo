#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <memory>

#include "Mesh.h"
#include "ModelLoader.h"

class Model
{
public:

    std::vector<std::unique_ptr<Mesh>> meshes;

    Model(const tinygltf::Model& gltfModel);

    void LoadMesh(const tinygltf::Model& gltfModel);

    void Draw();

};

#endif
