#include "GLTexture.h"

int GLTexture::getTextureMode()
{
	if (cubemap)
		return GL_TEXTURE_CUBE_MAP;

	if (threeDimensional)
		return GL_TEXTURE_3D;

	return GL_TEXTURE_2D;
}

GLTexture::GLTexture() : Texture()
{
	this->fileName = "";
	this->size = glm::vec2(0);
}

GLTexture::GLTexture(std::string filename) : Texture(filename)
{
	this->fileName = filename;
	this->type = textureType_e::color2D;

	//this->load(filename);
	this->update();
}

GLTexture::GLTexture(const std::string& left, const std::string& right,
	const std::string& front, const std::string& back,
	const std::string& top, const std::string& bottom)
{
	this->fileName = left;
	this->type = textureType_e::cubic;
	this->load(left, right,
		front, back,
		top, bottom);
	this->update();
}

GLTexture::GLTexture(const std::string& name, const std::string& format, int frame_range)
{
	this->fileName = name;
	this->type = textureType_e::color3D;
	this->load(name, format, frame_range);
	this->update();
}

void GLTexture::bind(size_t layer)
{
	glActiveTexture(GL_TEXTURE0 + layer);

	if (isCube())
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->glTexId);
	if (is3D())
		glBindTexture(GL_TEXTURE_3D, this->glTexId);
	else
		glBindTexture(GL_TEXTURE_2D, this->glTexId);
}

void GLTexture::update()
{
	//generar textura de opengl
	glGenTextures(1, &this->glTexId);

	int texMode = getTextureMode();

	glBindTexture(texMode, this->glTexId);

	//seteo par�metros
	//filtros
	if (bilinear)
	{
		glTexParameteri(texMode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(texMode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(texMode, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(texMode, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	if (repeat)
	{
		glTexParameteri(texMode, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(texMode, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (threeDimensional)
			glTexParameteri(texMode, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else
	{
		glTexParameteri(texMode, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(texMode, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		if (threeDimensional)
			glTexParameteri(texMode, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	}

	// NOTE a falta de saber como sera el formato de cubemap se queda sin generar.
	//cargar datos
	if (cubemap)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, texBytes[0].data());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, texBytes[1].data());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, texBytes[2].data());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, texBytes[3].data());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, texBytes[4].data());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, texBytes[5].data());
	}
	else if (threeDimensional)
	{
		// TODO estamos asumiendo texturas cubicas
		float depth = std::cbrtf(texBytes[0].size() / 4);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, size.x, size.y, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, texBytes[0].data());
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, texBytes[0].data());
	}

	//generar mipmaps
	glGenerateMipmap(texMode);
}

GLuint GLTexture::getTexId()
{
	return this->glTexId;
}
