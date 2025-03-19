#pragma once
#include "Material.h"
#include "VKProgram.h"
#include "VKTexture.h"


class VKMaterial : public Material
{
private:

public:
	VKMaterial();

	void copyValues(Material* mat) override {};

	void loadPrograms(std::vector<std::string> files) override;
	void prepare() override;

	void setTexture(std::string filename) override;
	void setTexture(std::string name, std::string filename, std::string type) override;
};