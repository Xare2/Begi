#include "GLTextureFB.h"

GLTextureFB::GLTextureFB(int type, int width, int height)
{
	this->fbID = GLTextureFB::fbCount++;
	this->type = (Texture::textureType_e)type;
	this->size = { width, height };

	glGenTextures(1, &this->glTexId);
	glBindTexture(GL_TEXTURE_2D, this->glTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	switch (type)
	{
	case colorBuffer:
	{
		//color
		//al reservar el espacio de la textura, no se inician los colores (nullptr)
		glTexImage2D(GL_TEXTURE_2D, 0,
			GL_RGBA, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE,
			nullptr
		);
	}
	break;
	case depthBuffer:
	{
		//profundidad
		glTexImage2D(
			GL_TEXTURE_2D, 0,
			GL_DEPTH_COMPONENT24,
			width, height,
			0, GL_DEPTH_COMPONENT,
			GL_UNSIGNED_BYTE, nullptr
		);
	}
	break;
	}
}

int GLTextureFB::getId()
{
	return this->fbID;
}
