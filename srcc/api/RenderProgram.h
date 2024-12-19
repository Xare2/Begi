#pragma once
#include "common.h"
#include "Program.h"
#include "Texture.h"
#include "Light.h"

class RenderProgram
{
protected:
	std::vector<Program*> shaders;
	std::map<std::string, unsigned int> varList;

	unsigned int programId = -1;

public:
	virtual void addProgram(std::string fileName) = 0;
	virtual void linkProgram() = 0;
	virtual void use() = 0;
	virtual void checkLinkerErrors() = 0;

	virtual void setInt(std::string name, int val) = 0;
	virtual void setFloat(std::string name, float val) = 0;
	virtual void setVec3(std::string name, const glm::vec3& vec) = 0;
	virtual void setVec4(std::string name, const glm::vec4& vec) = 0;
	virtual void setMatrix(std::string name, const glm::mat4& matrix) = 0;

	virtual void setLight(Light* l) = 0;
	virtual void setLight(Light* l, int idx) = 0;

	virtual void setColorTexEnable() = 0;
	virtual void setColorTexDisable() = 0;
	virtual void bindTextureSampler(int binding, Texture* text) = 0;
};