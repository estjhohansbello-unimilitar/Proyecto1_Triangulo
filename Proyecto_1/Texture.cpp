#include"Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	type = texType;

	// --------------------------------------------------------
	// Cargar la imagen desde disco con stb_image
	// --------------------------------------------------------

	int widthImg, heightImg, numColCh;

	// Voltea la imagen verticalmente, porque OpenGL espera que
	// el (0,0) de las coordenadas UV esté abajo-izquierda,
	// mientras que la mayoría de formatos de imagen lo guardan arriba-izquierda
	stbi_set_flip_vertically_on_load(true);

	// Forzamos 4 canales (RGBA) sin importar cuántos tenga la imagen original.
	// Esto es importante: si la imagen no tuviera alpha (solo RGB, 3 canales)
	// y aquí pidiéramos "los canales que tenga" (0), el buffer resultante
	// tendría menos bytes por píxel de los que luego se le dicen a OpenGL
	// que lea en glTexImage2D (GL_RGBA = 4 canales). Esa discordancia hace
	// que el driver lea memoria fuera del buffer -> crash de acceso inválido.
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 4);

	if (bytes == nullptr)
	{
		std::cout << "ERROR::TEXTURE No se pudo cargar la imagen: " << image << std::endl;
	}

	// --------------------------------------------------------
	// Generar la textura en OpenGL
	// --------------------------------------------------------

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	// Filtros de escalado (cómo se ve la textura al agrandarla/achicarla)
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Cómo se repite la textura si las UV se salen del rango 0-1
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Subir los píxeles a la GPU y generar mipmaps
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
	glGenerateMipmap(texType);

	// Ya no se necesitan los bytes en RAM, ni la textura ligada
	stbi_image_free(bytes);
	glBindTexture(texType, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}
