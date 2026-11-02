#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION



#include "tinygltf/tiny_gltf.h"
#include <iostream> 
#include <string>

bool LoadGLTF(const std::string& path,
    tinygltf::Model& model)
{
    tinygltf::TinyGLTF loader;

    std::string err;
    std::string warn;

    bool ret = false;

    if (path.size() >= 4 &&
        path.substr(path.size() - 4) == ".glb")
    {
        ret = loader.LoadBinaryFromFile(
            &model, &err, &warn, path
        );
    }
    else
    {
        ret = loader.LoadASCIIFromFile(
            &model, &err, &warn, path
        );
    }

    if (!warn.empty())
        std::cout << "Advertencia: " << warn << std::endl;

    if (!err.empty())
        std::cout << "Error: " << err << std::endl;

    if (!ret)
    {
        std::cout << "Error al cargar el modelo"
            << std::endl;
        return false;
    }

    std::cout << "Modelo cargado correctamente"
        << std::endl;

    return true;
}