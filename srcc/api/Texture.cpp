#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
{
	this->texID = texCount++;
	this->fileName = "";
	this->size = glm::ivec2(0);

	//texBytes = std::vector<unsigned char>();
}

Texture::Texture(std::string fileName)
{
	this->texID = texCount++;
	this->fileName = fileName;
	this->size = glm::ivec2(0);

	//texBytes = std::vector<unsigned char>();

	this->load(fileName);
}

Texture::textureType_e Texture::getType() const
{
	return this->type;
}

glm::ivec2 Texture::getSize() const
{
	return this->size;
}

bool Texture::isCube() const
{
	return this->cubemap;
}

bool Texture::is3D() const
{
	return this->threeDimensional;
}

void Texture::load(std::string fileName)
{
	int comp;
	int sizeX = 0, sizeY = 0;

	unsigned char* data = stbi_load(fileName.c_str(), &sizeX, &sizeY, &comp, 4);

	this->texBytes.resize(1);

	if (data == nullptr)
	{
		std::cout << __FILE__ << ":" << __LINE__ << " ERROR: fichero " << fileName << " no encontrado\n";
	}
	else
	{
		texBytes[0] = std::vector<unsigned char>(sizeX * sizeY * 4);
		memcpy(texBytes[0].data(), data, texBytes[0].size());
		stbi_image_free(data);

		size = glm::ivec2({ sizeX, sizeY });

		//update();
	}
}

void Texture::load(const std::string& left, const std::string& right,
	const std::string& front, const std::string& back,
	const std::string& top, const std::string& bottom)
{
	this->texBytes.resize(6);

	int comp;
	unsigned char* data[6];
	this->cubemap = true;
	data[0] = stbi_load(left.c_str(), &size.x, &size.y, &comp, 4);
	data[1] = stbi_load(right.c_str(), &size.x, &size.y, &comp, 4);
	data[2] = stbi_load(bottom.c_str(), &size.x, &size.y, &comp, 4);
	data[3] = stbi_load(top.c_str(), &size.x, &size.y, &comp, 4);
	data[4] = stbi_load(back.c_str(), &size.x, &size.y, &comp, 4);
	data[5] = stbi_load(front.c_str(), &size.x, &size.y, &comp, 4);

	for (int i = 0; i < 6; i++)
	{
		texBytes[i].resize(size.x * size.y * 4);
		memcpy(texBytes[i].data(), data[i], texBytes[i].size());
		stbi_image_free(data[i]);
	}

	this->cubemap = true;
	this->bilinear = true;
}

void Texture::load(const std::string& name, const std::string& format, int frame_range)
{
	this->texBytes.resize(1);

	int comp;
	unsigned char** data = new unsigned char* [frame_range];

	for (int i = 0; i < frame_range; i++)
	{
		std::string path = name + std::to_string(i + 1) + format;
		data[i] = stbi_load(path.c_str(), &size.x, &size.y, &comp, 4);
	}

	for (int i = 0; i < frame_range; i++) {
		this->texBytes[0].insert(this->texBytes[0].end(), data[i], data[i] + size.x * size.y * 4);

		stbi_image_free(data[i]);
	}

	this->threeDimensional = true;
}
