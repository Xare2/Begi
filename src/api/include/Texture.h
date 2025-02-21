#pragma once

#include "common.h"


class Texture
{
public:
	typedef enum textureType_e {
		color2D, color3D, cubic, colorBuffer, depthBuffer
	} textureType_e;

	inline static const std::string COLOR_2D = "color2D";
	inline static const std::string CUBE_MAP = "cubeMap";
	inline static const std::string REFLECTION = "reflection";
	inline static const std::string REFRACTION = "refraction";
	inline static const std::string NORMAL_MAP = "textureNormal";

	Texture();
	Texture(std::string fileName);

	textureType_e getType() const;
	glm::ivec2 getSize() const;
	bool isCube() const;
	bool is3D() const;

	void load(std::string fileName);
	void load(
		const std::string& left, const std::string& right,
		const std::string& front, const std::string& back,
		const std::string& top, const std::string& bottom
	);
	void load(const std::string& name, const std::string& format, int frame_range);

	virtual void update() = 0;

private:
	static inline glm::uint32_t texCount = 0;


protected:
	std::vector<std::vector<unsigned char>> texBytes;
	textureType_e type;

	std::string fileName;
	glm::uint32_t texID;
	glm::ivec2 size;
	bool bilinear = true;
	bool cubemap = false;
	bool repeat = true;
	bool threeDimensional = false;
};

