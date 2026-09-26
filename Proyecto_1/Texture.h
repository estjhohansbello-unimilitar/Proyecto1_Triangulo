#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include"stb_image.h"
#include"shaderClass.h"

class Texture
{
public:
	GLuint ID;
	GLenum type;
	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

	// Le dice al shader en qué "unidad de textura" (slot) buscar esta textura
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Activa la textura para usarla al dibujar
	void Bind();
	// Desactiva la textura
	void Unbind();
	// Libera la textura de la GPU
	void Delete();
};

#endif
