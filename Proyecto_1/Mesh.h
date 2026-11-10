
#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

// Información de cada vértice
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
	glm::vec2 texCoords;// coordenadas UV
};

class Mesh
{
public:

    VAO vao;

    std::unique_ptr<VBO> vbo;
    std::unique_ptr<EBO> ebo;

	std::vector<Vertex> vertices; // vector de vértices
    std::vector<GLuint> indices;
    
    int materialIndex = -1;

    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<GLuint>& indices
    );

    ~Mesh();

    void Draw();

    void Delete();
};

#endif

