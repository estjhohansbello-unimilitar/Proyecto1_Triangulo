
#include "Mesh.h"

#include <cstddef>

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<GLuint>& indices
)
{
    this->vertices = vertices;
    this->indices = indices;

    // Activar el VAO
    vao.Bind();

    // Crear el VBO
    vbo = std::make_unique<VBO>(
        reinterpret_cast<GLfloat*>(this->vertices.data()),
        this->vertices.size() * sizeof(Vertex)
    );

    // Crear el EBO
    ebo = std::make_unique<EBO>(
        this->indices.data(),
        this->indices.size() * sizeof(GLuint)
    );

    // Posición: location 0
    vao.LinkAttrib(
        *vbo,
        0,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)offsetof(Vertex, position)
    );

    // Normal: location 1
    vao.LinkAttrib(
        *vbo,
        1,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)offsetof(Vertex, normal)
    );

    // Coordenadas UV: location 2
    vao.LinkAttrib(
        *vbo,
        2,
        2,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)offsetof(Vertex, texCoords)
    );

    // Desactivar el VAO
    vao.Unbind();

    vbo->Unbind();
}

Mesh::~Mesh()
{
    Delete();
}

void Mesh::Draw()
{
    vao.Bind();

    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(indices.size()),
        GL_UNSIGNED_INT,
        0
    );

    vao.Unbind();
}

void Mesh::Delete()
{
    if (vao.ID != 0)
    {
        vao.Delete();
        vao.ID = 0;
    }

    if (vbo)
    {
        vbo->Delete();
        vbo.reset();
    }

    if (ebo)
    {
        ebo->Delete();
        ebo.reset();
    }
}
