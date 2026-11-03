
#include "Model.h"

#include <iostream>
#include <cstring>
#include <cstddef>

// Obtener la dirección de los datos de un accessor
static const unsigned char* GetAccessorData(
    const tinygltf::Model& model,
    const tinygltf::Accessor& accessor)
{
    const tinygltf::BufferView& view =
        model.bufferViews[accessor.bufferView];

    const tinygltf::Buffer& buffer =
        model.buffers[view.buffer];

    return buffer.data.data()
        + view.byteOffset
        + accessor.byteOffset;
}

// Obtener el tamaño de cada elemento
static size_t GetStride(
    const tinygltf::Model& model,
    const tinygltf::Accessor& accessor)
{
    const tinygltf::BufferView& view =
        model.bufferViews[accessor.bufferView];

    int stride = accessor.ByteStride(view);

    if (stride < 0)
        return 0;

    return static_cast<size_t>(stride);
}

// Leer un vector de tres componentes FLOAT
static glm::vec3 ReadVec3(
    const unsigned char* data,
    size_t stride,
    size_t index)
{
    glm::vec3 result;

    std::memcpy(
        &result,
        data + index * stride,
        sizeof(float) * 3
    );

    return result;
}

// Leer coordenadas UV FLOAT
static glm::vec2 ReadVec2(
    const unsigned char* data,
    size_t stride,
    size_t index)
{
    glm::vec2 result;

    std::memcpy(
        &result,
        data + index * stride,
        sizeof(float) * 2
    );

    return result;
}

// Constructor
Model::Model(const tinygltf::Model& gltfModel)
{
    LoadMesh(gltfModel);
}

// Extraer las mallas del modelo
void Model::LoadMesh(const tinygltf::Model& gltfModel)
{
    for (const auto& gltfMesh : gltfModel.meshes)
    {
        for (const auto& primitive : gltfMesh.primitives)
        {
            // Por ahora solo trabajamos con triángulos
            if (primitive.mode != -1 &&
                primitive.mode != TINYGLTF_MODE_TRIANGLES)
            {
                std::cout << "Primitiva omitida: no utiliza triangulos"
                    << std::endl;
                continue;
            }

            // Buscar posiciones
            auto positionIt =
                primitive.attributes.find("POSITION");

            if (positionIt == primitive.attributes.end())
            {
                std::cout << "La primitiva no tiene posiciones"
                    << std::endl;
                continue;
            }

            const tinygltf::Accessor& positionAccessor =
                gltfModel.accessors[positionIt->second];

            if (positionAccessor.componentType !=
                TINYGLTF_COMPONENT_TYPE_FLOAT ||
                positionAccessor.type != TINYGLTF_TYPE_VEC3)
            {
                std::cout << "Formato de POSITION no compatible"
                    << std::endl;
                continue;
            }

            const unsigned char* positionData =
                GetAccessorData(gltfModel, positionAccessor);

            size_t positionStride =
                GetStride(gltfModel, positionAccessor);

            size_t vertexCount = positionAccessor.count;

            std::vector<Vertex> vertices(vertexCount);

            // Extraer posiciones
            for (size_t i = 0; i < vertexCount; i++)
            {
                vertices[i].position =
                    ReadVec3(positionData, positionStride, i);

                vertices[i].normal = glm::vec3(0.0f);
                vertices[i].texCoords = glm::vec2(0.0f);
            }

            // Extraer normales si existen
            auto normalIt =
                primitive.attributes.find("NORMAL");

            if (normalIt != primitive.attributes.end())
            {
                const tinygltf::Accessor& normalAccessor =
                    gltfModel.accessors[normalIt->second];

                if (normalAccessor.count == vertexCount &&
                    normalAccessor.componentType ==
                    TINYGLTF_COMPONENT_TYPE_FLOAT &&
                    normalAccessor.type == TINYGLTF_TYPE_VEC3)
                {
                    const unsigned char* normalData =
                        GetAccessorData(gltfModel, normalAccessor);

                    size_t normalStride =
                        GetStride(gltfModel, normalAccessor);

                    for (size_t i = 0; i < vertexCount; i++)
                    {
                        vertices[i].normal =
                            ReadVec3(normalData, normalStride, i);
                    }
                }
            }

            // Extraer coordenadas UV si existen
            auto uvIt =
                primitive.attributes.find("TEXCOORD_0");

            if (uvIt != primitive.attributes.end())
            {
                const tinygltf::Accessor& uvAccessor =
                    gltfModel.accessors[uvIt->second];

                if (uvAccessor.count == vertexCount &&
                    uvAccessor.componentType ==
                    TINYGLTF_COMPONENT_TYPE_FLOAT &&
                    uvAccessor.type == TINYGLTF_TYPE_VEC2)
                {
                    const unsigned char* uvData =
                        GetAccessorData(gltfModel, uvAccessor);

                    size_t uvStride =
                        GetStride(gltfModel, uvAccessor);

                    for (size_t i = 0; i < vertexCount; i++)
                    {
                        vertices[i].texCoords =
                            ReadVec2(uvData, uvStride, i);
                    }
                }
            }

            // Extraer índices
            std::vector<GLuint> indices;

            if (primitive.indices >= 0)
            {
                const tinygltf::Accessor& indexAccessor =
                    gltfModel.accessors[primitive.indices];

                const unsigned char* indexData =
                    GetAccessorData(gltfModel, indexAccessor);

                size_t indexStride =
                    GetStride(gltfModel, indexAccessor);

                indices.resize(indexAccessor.count);

                bool validIndices = true;

                for (size_t i = 0; i < indexAccessor.count; i++)
                {
                    const unsigned char* current =
                        indexData + i * indexStride;

                    switch (indexAccessor.componentType)
                    {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        indices[i] = *current;
                        break;

                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    {
                        unsigned short value;
                        std::memcpy(&value, current, sizeof(value));
                        indices[i] = value;
                        break;
                    }

                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    {
                        unsigned int value;
                        std::memcpy(&value, current, sizeof(value));
                        indices[i] = value;
                        break;
                    }

                    default:
                        validIndices = false;
                        break;
                    }

                    if (!validIndices)
                        break;
                }

                if (!validIndices)
                {
                    std::cout << "Tipo de indice no compatible"
                        << std::endl;
                    continue;
                }
            }
            else
            {
                for (GLuint i = 0; i < vertexCount; i++)
                {
                    indices.push_back(i);
                }
            }
           

            if (indices.empty())
                continue;

            // Crear la malla de OpenGL
            meshes.push_back(
                std::make_unique<Mesh>(vertices, indices)
            );

            std::cout << "Malla creada correctamente. Vertices: "
                << vertices.size()
                << " | Indices: "
                << indices.size()
                << std::endl;
        }
       
    }
    // INFORMACIÓN DEL MODELO
    std::cout << "\n========== INFORMACION DEL MODELO ==========" << std::endl;

    std::cout << "Cantidad de meshes: "
        << gltfModel.meshes.size() << std::endl;

    std::cout << "Cantidad de nodos: "
        << gltfModel.nodes.size() << std::endl;

    std::cout << "Cantidad de escenas: "
        << gltfModel.scenes.size() << std::endl;

    std::cout << "Cantidad de mallas OpenGL creadas: "
        << meshes.size() << std::endl;

    std::cout << "============================================" << std::endl;

    // INFORMACION DE LOS NODOS

    std::cout << "\n========== INFORMACION DE NODOS ==========" << std::endl;

    for (size_t i = 0; i < gltfModel.nodes.size(); i++)
    {
        const auto& node = gltfModel.nodes[i];

        std::cout << "\nNodo: " << i << std::endl;
        std::cout << "Nombre: " << node.name << std::endl;
        std::cout << "Mesh asociado: " << node.mesh << std::endl;

        std::cout << "Hijos: ";

        for (int child : node.children)
        {
            std::cout << child << " ";
        }

        std::cout << std::endl;

        // TRANSFORMACIONES DEL NODO

        std::cout << "Translation: ";

        for (double value : node.translation)
        {
            std::cout << value << " ";
        }

        std::cout << "\nScale: ";

        for (double value : node.scale)
        {
            std::cout << value << " ";
        }

        std::cout << "\nRotation: ";

        for (double value : node.rotation)
        {
            std::cout << value << " ";
        }

        std::cout << "\nMatrix: ";

        for (double value : node.matrix)
        {
            std::cout << value << " ";
        }

        std::cout << "\n";
    }

    std::cout << "==========================================" << std::endl;
        
}

// Dibujar todas las mallas
void Model::Draw()
{
    for (auto& mesh : meshes)
    {
        mesh->Draw();
    }
}
