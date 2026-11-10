
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tinygltf/tiny_gltf.h"
#include <cctype>
#include <iostream>
#include <string>
#include <algorithm>

bool LoadGLTF(const std::string& path, tinygltf::Model& model)
{
    tinygltf::TinyGLTF loader;

    std::string err;
    std::string warn;

    bool ret = false;

    // Obtener la extensión del archivo
    std::string extension;

    size_t dotPosition = path.find_last_of('.');

    if (dotPosition != std::string::npos)
    {
        extension = path.substr(dotPosition);
    }

    // Convertir la extensión a minúsculas
    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); }
    );

    // Cargar según el formato
    if (extension == ".glb")
    {
        std::cout << "Cargando archivo GLB: " << path << std::endl;

        ret = loader.LoadBinaryFromFile(
            &model,
            &err,
            &warn,
            path
        );
    }
    else if (extension == ".gltf")
    {
        std::cout << "Cargando archivo glTF: " << path << std::endl;

        ret = loader.LoadASCIIFromFile(
            &model,
            &err,
            &warn,
            path
        );
    }
    else
    {
        std::cout << "Formato de archivo no compatible: "
            << extension << std::endl;

        return false;
    }

    // Mostrar advertencias
    if (!warn.empty())
    {
        std::cout << "Advertencia: " << warn << std::endl;
    }

    // Mostrar errores
    if (!err.empty())
    {
        std::cout << "Error: " << err << std::endl;
    }

    if (!ret)
    {
        std::cout << "Error al cargar el modelo" << std::endl;
        return false;
    }

    std::cout << "Modelo cargado correctamente" << std::endl;

    return true;
}
