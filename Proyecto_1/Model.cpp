
#include "Model.h"

#include <iostream>
#include <cstring>
#include <cstddef>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

// Dirección de los datos de un accessor.
static const unsigned char* GetAccessorData(
    const tinygltf::Model& model,
    const tinygltf::Accessor& accessor)
{
    if (accessor.bufferView < 0 ||
        accessor.bufferView >= (int)model.bufferViews.size())
    {
        return nullptr;
    }

    const tinygltf::BufferView& view =
        model.bufferViews[accessor.bufferView];

    if (view.buffer < 0 ||
        view.buffer >= (int)model.buffers.size())
    {
        return nullptr;
    }

    const tinygltf::Buffer& buffer =
        model.buffers[view.buffer];

    size_t offset = view.byteOffset + accessor.byteOffset;

    if (offset >= buffer.data.size())
        return nullptr;

    return buffer.data.data() + offset;
}


// Separación en bytes entre elementos consecutivos.
static size_t GetStride(
    const tinygltf::Model& model,
    const tinygltf::Accessor& accessor)
{
    if (accessor.bufferView < 0 ||
        accessor.bufferView >= (int)model.bufferViews.size())
    {
        return 0;
    }

    const tinygltf::BufferView& view =
        model.bufferViews[accessor.bufferView];

    int stride = accessor.ByteStride(view);

    if (stride < 0)
        return 0;

    return static_cast<size_t>(stride);
}


// Leer un vector de tres componentes.
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


// Leer un vector de dos componentes.
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


// ---------------------------------------------------------------
// CREAR UNA TEXTURA OPENGL A PARTIR DE UNA TEXTURA GLTF
// ---------------------------------------------------------------

static GLuint CreateGLTexture(
    const tinygltf::Model& model,
    int textureIndex)
{
    if (textureIndex < 0 ||
        textureIndex >= (int)model.textures.size())
    {
        return 0;
    }

    const tinygltf::Texture& tex =
        model.textures[textureIndex];

    if (tex.source < 0 ||
        tex.source >= (int)model.images.size())
    {
        std::cout << "Textura " << textureIndex
            << ": no tiene imagen asociada"
            << std::endl;

        return 0;
    }

    const tinygltf::Image& img =
        model.images[tex.source];

    if (img.image.empty() ||
        img.width <= 0 ||
        img.height <= 0)
    {
        std::cout << "Textura " << textureIndex
            << ": imagen sin datos decodificados ("
            << img.uri << ")"
            << std::endl;

        return 0;
    }

    if (img.bits != 8)
    {
        std::cout << "Textura " << textureIndex
            << ": solo se soportan imagenes de 8 bits"
            << std::endl;

        return 0;
    }

    GLenum format;
    GLint internalFormat;

    if (img.component == 4)
    {
        format = GL_RGBA;
        internalFormat = GL_RGBA8;
    }
    else if (img.component == 3)
    {
        format = GL_RGB;
        internalFormat = GL_RGB8;
    }
    else
    {
        std::cout << "Textura " << textureIndex
            << ": numero de canales no soportado ("
            << img.component << ")"
            << std::endl;

        return 0;
    }

    // Configuración de repetición.
    GLint wrapS = GL_REPEAT;
    GLint wrapT = GL_REPEAT;

    if (tex.sampler >= 0 &&
        tex.sampler < (int)model.samplers.size())
    {
        const tinygltf::Sampler& s =
            model.samplers[tex.sampler];

        if (s.wrapS > 0)
            wrapS = s.wrapS;

        if (s.wrapT > 0)
            wrapT = s.wrapT;
    }

    GLuint id = 0;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        img.width,
        img.height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        img.image.data()
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        wrapS
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        wrapT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Textura " << textureIndex
        << " creada: "
        << img.width << "x" << img.height
        << " (" << img.component << " canales)"
        << std::endl;

    return id;
}


// ---------------------------------------------------------------
// CONSTRUCTOR Y DESTRUCTOR
// ---------------------------------------------------------------

Model::Model(const tinygltf::Model& gltfModel)
{
    LoadMaterials(gltfModel);

    LoadMesh(gltfModel);

    // Calcular los límites reales considerando los nodos.
    CalculateBounds(gltfModel);
}


Model::~Model()
{
    for (GLuint t : glTextures)
    {
        if (t != 0)
            glDeleteTextures(1, &t);
    }
}


// ---------------------------------------------------------------
// CARGAR MATERIALES Y TEXTURAS
// ---------------------------------------------------------------

void Model::LoadMaterials(
    const tinygltf::Model& gltfModel)
{
    glTextures.assign(gltfModel.textures.size(), 0);

    textureTried.assign(gltfModel.textures.size(), 0);

    // Crear cada textura una sola vez.
    auto getTexture = [&](int index) -> GLuint
        {
            if (index < 0 ||
                index >= (int)glTextures.size())
            {
                return 0;
            }

            if (!textureTried[index])
            {
                textureTried[index] = 1;

                glTextures[index] =
                    CreateGLTexture(gltfModel, index);
            }

            return glTextures[index];
        };

    materials.clear();

    for (size_t i = 0;
        i < gltfModel.materials.size();
        i++)
    {
        const tinygltf::Material& mat =
            gltfModel.materials[i];

        const auto& pbr =
            mat.pbrMetallicRoughness;

        MaterialData data;

        // Color base.
        if (pbr.baseColorFactor.size() == 4)
        {
            data.baseColor = glm::vec4(
                (float)pbr.baseColorFactor[0],
                (float)pbr.baseColorFactor[1],
                (float)pbr.baseColorFactor[2],
                (float)pbr.baseColorFactor[3]
            );
        }

        // Textura de color base.
        if (pbr.baseColorTexture.index >= 0)
        {
            if (pbr.baseColorTexture.texCoord != 0)
            {
                std::cout
                    << "Material " << i
                    << ": utiliza TEXCOORD_"
                    << pbr.baseColorTexture.texCoord
                    << ". Solo se soporta TEXCOORD_0."
                    << std::endl;
            }

            data.texture =
                getTexture(pbr.baseColorTexture.index);
        }

        // Tratamiento básico de transparencia.
        if (mat.alphaMode == "MASK")
        {
            data.alphaCutoff =
                (float)mat.alphaCutoff;
        }
        else if (mat.alphaMode == "BLEND")
        {
            data.alphaCutoff = 0.05f;
        }
        else
        {
            data.alphaCutoff = 0.0f;
        }

        std::cout
            << "Material " << i
            << " (" << mat.name << "): "
            << (data.texture != 0
                ? "con textura"
                : "sin textura")
            << " | alphaMode: "
            << mat.alphaMode
            << std::endl;

        materials.push_back(data);
    }
}


// ---------------------------------------------------------------
// CARGAR MALLAS GLTF Y CREAR MALLAS OPENGL
// ---------------------------------------------------------------

void Model::LoadMesh(
    const tinygltf::Model& gltfModel)
{
    // Una entrada por cada malla original del glTF.
    meshPrimitiveMap.clear();

    meshPrimitiveMap.resize(
        gltfModel.meshes.size()
    );

    meshes.clear();

    for (size_t meshIndex = 0;
        meshIndex < gltfModel.meshes.size();
        meshIndex++)
    {
        const auto& gltfMesh =
            gltfModel.meshes[meshIndex];

        for (const auto& primitive : gltfMesh.primitives)
        {
            // Solo trabajar con triángulos.
            if (primitive.mode != -1 &&
                primitive.mode != TINYGLTF_MODE_TRIANGLES)
            {
                std::cout
                    << "Primitiva omitida: no utiliza triangulos"
                    << std::endl;

                continue;
            }

            // Buscar posiciones.
            auto positionIt =
                primitive.attributes.find("POSITION");

            if (positionIt == primitive.attributes.end())
            {
                std::cout
                    << "La primitiva no tiene posiciones"
                    << std::endl;

                continue;
            }

            const tinygltf::Accessor& positionAccessor =
                gltfModel.accessors[positionIt->second];

            if (positionAccessor.componentType !=
                TINYGLTF_COMPONENT_TYPE_FLOAT ||
                positionAccessor.type != TINYGLTF_TYPE_VEC3)
            {
                std::cout
                    << "Formato de POSITION no compatible"
                    << std::endl;

                continue;
            }

            const unsigned char* positionData =
                GetAccessorData(
                    gltfModel,
                    positionAccessor
                );

            if (positionData == nullptr)
            {
                std::cout
                    << "POSITION sin bufferView"
                    << std::endl;

                continue;
            }

            size_t positionStride =
                GetStride(gltfModel, positionAccessor);

            size_t vertexCount =
                positionAccessor.count;

            if (positionStride < sizeof(float) * 3)
            {
                std::cout
                    << "Stride de POSITION no valido"
                    << std::endl;

                continue;
            }

            std::vector<Vertex> vertices(vertexCount);

            // Extraer posiciones.
            for (size_t i = 0; i < vertexCount; i++)
            {
                vertices[i].position =
                    ReadVec3(
                        positionData,
                        positionStride,
                        i
                    );

                vertices[i].normal =
                    glm::vec3(0.0f, 1.0f, 0.0f);

                vertices[i].texCoords =
                    glm::vec2(0.0f);
            }

            // ---------------------------------------------------
            // EXTRAER NORMALES
            // ---------------------------------------------------

            auto normalIt =
                primitive.attributes.find("NORMAL");

            if (normalIt != primitive.attributes.end())
            {
                const tinygltf::Accessor& normalAccessor =
                    gltfModel.accessors[normalIt->second];

                const unsigned char* normalData =
                    GetAccessorData(
                        gltfModel,
                        normalAccessor
                    );

                if (normalData != nullptr &&
                    normalAccessor.count == vertexCount &&
                    normalAccessor.componentType ==
                    TINYGLTF_COMPONENT_TYPE_FLOAT &&
                    normalAccessor.type == TINYGLTF_TYPE_VEC3)
                {
                    size_t normalStride =
                        GetStride(
                            gltfModel,
                            normalAccessor
                        );

                    if (normalStride >= sizeof(float) * 3)
                    {
                        for (size_t i = 0; i < vertexCount; i++)
                        {
                            vertices[i].normal =
                                ReadVec3(
                                    normalData,
                                    normalStride,
                                    i
                                );
                        }
                    }
                }
            }

            // ---------------------------------------------------
            // EXTRAER COORDENADAS UV
            // ---------------------------------------------------

            auto uvIt =
                primitive.attributes.find("TEXCOORD_0");

            if (uvIt != primitive.attributes.end())
            {
                const tinygltf::Accessor& uvAccessor =
                    gltfModel.accessors[uvIt->second];

                const unsigned char* uvData =
                    GetAccessorData(
                        gltfModel,
                        uvAccessor
                    );

                if (uvData != nullptr &&
                    uvAccessor.count == vertexCount &&
                    uvAccessor.componentType ==
                    TINYGLTF_COMPONENT_TYPE_FLOAT &&
                    uvAccessor.type == TINYGLTF_TYPE_VEC2)
                {
                    size_t uvStride =
                        GetStride(
                            gltfModel,
                            uvAccessor
                        );

                    if (uvStride >= sizeof(float) * 2)
                    {
                        for (size_t i = 0; i < vertexCount; i++)
                        {
                            vertices[i].texCoords =
                                ReadVec2(
                                    uvData,
                                    uvStride,
                                    i
                                );
                        }
                    }
                }
                else
                {
                    std::cout
                        << "TEXCOORD_0 en formato no soportado"
                        << std::endl;
                }
            }

            // ---------------------------------------------------
            // EXTRAER INDICES
            // ---------------------------------------------------

            std::vector<GLuint> indices;

            if (primitive.indices >= 0)
            {
                const tinygltf::Accessor& indexAccessor =
                    gltfModel.accessors[primitive.indices];

                const unsigned char* indexData =
                    GetAccessorData(
                        gltfModel,
                        indexAccessor
                    );

                if (indexData == nullptr)
                {
                    std::cout
                        << "Indices sin bufferView"
                        << std::endl;

                    continue;
                }

                size_t indexStride =
                    GetStride(
                        gltfModel,
                        indexAccessor
                    );

                if (indexStride == 0)
                {
                    std::cout
                        << "Stride de indices no valido"
                        << std::endl;

                    continue;
                }

                indices.resize(indexAccessor.count);

                bool validIndices = true;

                for (size_t i = 0;
                    i < indexAccessor.count;
                    i++)
                {
                    const unsigned char* current =
                        indexData + i * indexStride;

                    switch (indexAccessor.componentType)
                    {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    {
                        indices[i] = *current;
                        break;
                    }

                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    {
                        unsigned short value;

                        std::memcpy(
                            &value,
                            current,
                            sizeof(value)
                        );

                        indices[i] = value;
                        break;
                    }

                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    {
                        unsigned int value;

                        std::memcpy(
                            &value,
                            current,
                            sizeof(value)
                        );

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
                    std::cout
                        << "Tipo de indice no compatible"
                        << std::endl;

                    continue;
                }
            }
            else
            {
                for (GLuint i = 0;
                    i < vertexCount;
                    i++)
                {
                    indices.push_back(i);
                }
            }

            if (indices.empty())
                continue;

            // ---------------------------------------------------
            // CREAR MALLA OPENGL
            // ---------------------------------------------------

            // Guardar la posición de la malla OpenGL.
            size_t openGLMeshIndex = meshes.size();

            meshes.push_back(
                std::make_unique<Mesh>(
                    vertices,
                    indices
                )
            );

            meshes.back()->materialIndex =
                primitive.material;

            // Relacionar la primitiva con su malla glTF.
            meshPrimitiveMap[meshIndex].push_back(
                openGLMeshIndex
            );

            std::cout
                << "Malla OpenGL creada."
                << " Mesh glTF: " << meshIndex
                << " | Vertices: " << vertices.size()
                << " | Indices: " << indices.size()
                << " | Material: " << primitive.material
                << std::endl;
        }
    }

    // -----------------------------------------------------------
    // INFORMACION GENERAL DEL MODELO
    // -----------------------------------------------------------

    std::cout
        << "\n========== INFORMACION DEL MODELO =========="
        << std::endl;

    std::cout
        << "Cantidad de meshes glTF: "
        << gltfModel.meshes.size()
        << std::endl;

    std::cout
        << "Cantidad de nodos: "
        << gltfModel.nodes.size()
        << std::endl;

    std::cout
        << "Cantidad de escenas: "
        << gltfModel.scenes.size()
        << std::endl;

    std::cout
        << "Cantidad de mallas OpenGL creadas: "
        << meshes.size()
        << std::endl;

    std::cout
        << "Cantidad de materiales: "
        << materials.size()
        << std::endl;

    std::cout
        << "============================================"
        << std::endl;
}


// ---------------------------------------------------------------
// TRANSFORMACION LOCAL DE UN NODO
// ---------------------------------------------------------------

glm::mat4 Model::GetNodeTransform(
    const tinygltf::Node& node) const
{
    glm::mat4 local(1.0f);

    // Si existe una matriz, tiene prioridad sobre TRS.
    if (node.matrix.size() == 16)
    {
        for (int col = 0; col < 4; col++)
        {
            for (int row = 0; row < 4; row++)
            {
                local[col][row] =
                    (float)node.matrix[col * 4 + row];
            }
        }

        return local;
    }

    // TRASLACION.
    if (node.translation.size() == 3)
    {
        glm::vec3 translation(
            (float)node.translation[0],
            (float)node.translation[1],
            (float)node.translation[2]
        );

        local = glm::translate(
            local,
            translation
        );
    }

    // ROTACION.
    // glTF almacena el cuaternion como x, y, z, w.
    if (node.rotation.size() == 4)
    {
        glm::quat rotation(
            (float)node.rotation[3],
            (float)node.rotation[0],
            (float)node.rotation[1],
            (float)node.rotation[2]
        );

        local *= glm::mat4_cast(rotation);
    }

    // ESCALA.
    if (node.scale.size() == 3)
    {
        glm::vec3 scale(
            (float)node.scale[0],
            (float)node.scale[1],
            (float)node.scale[2]
        );

        local = glm::scale(
            local,
            scale
        );
    }

    return local;
}


// ---------------------------------------------------------------
// CALCULAR LIMITES REALES DE LOS NODOS
// ---------------------------------------------------------------

void Model::CalculateNodeBounds(
    const tinygltf::Model& gltfModel,
    int nodeIndex,
    const glm::mat4& parentTransform)
{
    if (nodeIndex < 0 ||
        nodeIndex >= (int)gltfModel.nodes.size())
    {
        return;
    }

    const tinygltf::Node& node =
        gltfModel.nodes[nodeIndex];

    glm::mat4 globalTransform =
        parentTransform * GetNodeTransform(node);

    // Comprobar si el nodo tiene una malla.
    if (node.mesh >= 0 &&
        node.mesh < (int)meshPrimitiveMap.size())
    {
        for (size_t meshIndex :
        meshPrimitiveMap[node.mesh])
        {
            const Mesh& mesh =
                *meshes[meshIndex];

            for (const Vertex& vertex : mesh.vertices)
            {
                glm::vec4 transformed =
                    globalTransform *
                    glm::vec4(
                        vertex.position,
                        1.0f
                    );

                glm::vec3 position(
                    transformed.x,
                    transformed.y,
                    transformed.z
                );

                boundsMin =
                    glm::min(boundsMin, position);

                boundsMax =
                    glm::max(boundsMax, position);
            }
        }
    }

    // Recorrer los hijos.
    for (int child : node.children)
    {
        CalculateNodeBounds(
            gltfModel,
            child,
            globalTransform
        );
    }
}


// ---------------------------------------------------------------
// INICIAR CALCULO DE LIMITES DEL ESCENARIO
// ---------------------------------------------------------------

void Model::CalculateBounds(
    const tinygltf::Model& gltfModel)
{
    boundsMin = glm::vec3(1e30f);
    boundsMax = glm::vec3(-1e30f);

    if (gltfModel.scenes.empty())
    {
        std::cout
            << "El archivo no contiene escenas."
            << std::endl;

        return;
    }

    int sceneIndex = gltfModel.defaultScene;

    if (sceneIndex < 0 ||
        sceneIndex >= (int)gltfModel.scenes.size())
    {
        sceneIndex = 0;
    }

    const tinygltf::Scene& scene =
        gltfModel.scenes[sceneIndex];

    // Comenzar desde los nodos raíz.
    for (int rootNode : scene.nodes)
    {
        CalculateNodeBounds(
            gltfModel,
            rootNode,
            glm::mat4(1.0f)
        );
    }

    std::cout
        << "\n========== LIMITES DEL ESCENARIO =========="
        << std::endl;

    std::cout
        << "Bounds Min: "
        << boundsMin.x << " "
        << boundsMin.y << " "
        << boundsMin.z
        << std::endl;

    std::cout
        << "Bounds Max: "
        << boundsMax.x << " "
        << boundsMax.y << " "
        << boundsMax.z
        << std::endl;

    std::cout
        << "============================================"
        << std::endl;
}


// ---------------------------------------------------------------
// DIBUJAR UN NODO Y SUS HIJOS
// ---------------------------------------------------------------

void Model::DrawNode(
    GLuint shaderID,
    const tinygltf::Model& gltfModel,
    int nodeIndex,
    const glm::mat4& parentTransform,
    const glm::mat4& viewProjection,
    const glm::mat4& sceneTransform)
{
    if (nodeIndex < 0 ||
        nodeIndex >= (int)gltfModel.nodes.size())
    {
        return;
    }

    const tinygltf::Node& node =
        gltfModel.nodes[nodeIndex];

    // Transformación acumulada.
    glm::mat4 globalTransform =
        parentTransform * GetNodeTransform(node);

    // Transformación final en el mundo.
    glm::mat4 modelMatrix =
        sceneTransform * globalTransform;

    // -----------------------------------------------------------
    // DIBUJAR MALLA ASOCIADA AL NODO
    // -----------------------------------------------------------

    if (node.mesh >= 0 &&
        node.mesh < (int)meshPrimitiveMap.size())
    {
        for (size_t meshIndex :
        meshPrimitiveMap[node.mesh])
        {
            Mesh& mesh =
                *meshes[meshIndex];

            // Material por defecto.
            MaterialData fallback;

            const MaterialData* material =
                &fallback;

            if (mesh.materialIndex >= 0 &&
                mesh.materialIndex < (int)materials.size())
            {
                material =
                    &materials[mesh.materialIndex];
            }

            // Obtener uniforms.
            GLint locColor =
                glGetUniformLocation(
                    shaderID,
                    "uBaseColor"
                );

            GLint locHasTex =
                glGetUniformLocation(
                    shaderID,
                    "uHasTexture"
                );

            GLint locCutoff =
                glGetUniformLocation(
                    shaderID,
                    "uAlphaCutoff"
                );

            GLint locTex =
                glGetUniformLocation(
                    shaderID,
                    "uTex"
                );

            GLint locModel =
                glGetUniformLocation(
                    shaderID,
                    "uModel"
                );

            GLint locMVP =
                glGetUniformLocation(
                    shaderID,
                    "uMVP"
                );

            // Calcular MVP.
            glm::mat4 mvp =
                viewProjection * modelMatrix;

            glUniformMatrix4fv(
                locModel,
                1,
                GL_FALSE,
                glm::value_ptr(modelMatrix)
            );

            glUniformMatrix4fv(
                locMVP,
                1,
                GL_FALSE,
                glm::value_ptr(mvp)
            );

            // Enviar material.
            glUniform4f(
                locColor,
                material->baseColor.r,
                material->baseColor.g,
                material->baseColor.b,
                material->baseColor.a
            );

            glUniform1i(
                locHasTex,
                material->texture != 0 ? 1 : 0
            );

            glUniform1f(
                locCutoff,
                material->alphaCutoff
            );

            // Activar textura.
            glActiveTexture(GL_TEXTURE0);

            glUniform1i(locTex, 0);

            glBindTexture(
                GL_TEXTURE_2D,
                material->texture
            );

            // Dibujar geometría.
            mesh.Draw();
        }
    }

    // -----------------------------------------------------------
    // RECORRER LOS HIJOS DEL NODO
    // -----------------------------------------------------------

    for (int child : node.children)
    {
        DrawNode(
            shaderID,
            gltfModel,
            child,
            globalTransform,
            viewProjection,
            sceneTransform
        );
    }
}


// ---------------------------------------------------------------
// DIBUJAR EL ESCENARIO COMPLETO
// ---------------------------------------------------------------

void Model::Draw(
    GLuint shaderID,
    const tinygltf::Model& gltfModel,
    const glm::mat4& viewProjection,
    const glm::mat4& sceneTransform)
{
    if (gltfModel.scenes.empty())
        return;

    int sceneIndex = gltfModel.defaultScene;

    if (sceneIndex < 0 ||
        sceneIndex >= (int)gltfModel.scenes.size())
    {
        sceneIndex = 0;
    }

    const tinygltf::Scene& scene =
        gltfModel.scenes[sceneIndex];

    // Dibujar desde los nodos raíz.
    for (int rootNode : scene.nodes)
    {
        DrawNode(
            shaderID,
            gltfModel,
            rootNode,
            glm::mat4(1.0f),
            viewProjection,
            sceneTransform
        );
    }

    // Liberar la textura vinculada.
    glBindTexture(GL_TEXTURE_2D, 0);
}
