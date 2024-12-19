#pragma once
#include "GLProgram.h"
#include "VKProgram.h"
#include "GLTexture.h"
#include "VKTexture.h"


class Material
{

public:
	enum blendMode_e {
		NONE, ALPHA, ADD, MUL
	};

	glm::vec4 color;

	virtual void copyValues(Material* mat) = 0;

	virtual void loadPrograms(std::vector<std::string> files) = 0;
	virtual void prepare() = 0;

	virtual void setTexture(std::string filename) = 0;
	virtual void setTexture(std::string name, std::string filename, std::string type) = 0;
	
	RenderProgram* getProgram() { return program; };
	Texture* getTexture();
	Texture* getTexture(std::string name);
	std::map<std::string, Texture*> getTextures();
	void setTexture(std::string name, Texture* tex);

	uint8_t getShininess() const;
	void setShininess(uint8_t shininess);

	blendMode_e getBlendMode() const;
	void setBlendMode(blendMode_e blendMode);

	bool getLighting() const;
	void setLighting(bool enable);

	bool getCulling() const;
	void setCulling(bool enable);

	bool getDepthWrite() const;
	void setDepthWrite(bool enable);

	bool getReflectionEnabled() const;
	void setReflectionEnabled(bool enabled);

	bool getRefractionEnabled() const;
	void setRefractionEnabled(bool enabled);

	float getRefractionIndex() const;
	void setRefractionIndex(float value);

	bool getShadowEnabled() const;
	void setShadowEnabled(bool enabled);

	void setIsMirror(bool value);

protected:
	std::map<std::string, Texture*> textures;

	RenderProgram* program = nullptr;
	//Texture* texture = nullptr;
	blendMode_e blendMode;
	uint8_t shininess;
	bool depthWrite;
	bool lighting;
	bool culling;

	bool reflectionEnabled;
	bool refractionEnabled;
	float refractIndex = 1;
	bool shadowEnabled;

	bool isMirror;
};

class GLSLMaterial : public Material
{

public:
	GLSLMaterial();
	GLSLMaterial(std::string vert, std::string frag);

	void copyValues(Material* from) override;

	void loadPrograms(std::vector<std::string> files) override;
	void prepare() override;

	void setTexture(std::string filename) override;
	void setTexture(std::string name, std::string filename, std::string type) override;
	
	GLTexture* getReflectionTexture() const;
	void setReflectionTexture(GLTexture* tex);

	GLTexture* getRefractionTexture() const;
	void setRefractionTexture(GLTexture* tex);

	GLTexture* getNormalTexture() const;
	void setNormalTexture(GLTexture* tex);

	float getRefractionCoef() const;
	void setRefractionCoef(float coef);
};

class GLSLPostProcessMaterial : public GLSLMaterial
{
private:

public:
	GLSLPostProcessMaterial();
	void prepare() override;
};

class GLSLBoxCloudMaterial : public GLSLMaterial
{
public:
	GLSLBoxCloudMaterial();
	void prepare() override;
};

class VKMaterial : public Material
{
private:

public:
	VKMaterial();

	void copyValues(Material* mat) override {};

	void loadPrograms(std::vector<std::string> files) override;
	void prepare() override;

	void setTexture(std::string filename) override;
	void setTexture(std::string name, std::string filename, std::string type) override {};
};