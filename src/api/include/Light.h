#pragma once
#include "Entity.h"

enum lightType_e {
	point, directional, spotLight
};

//struct light_t {
//	int enable;
//	int type;
//	float attenuation;
//	float cutOff;
//	alignas(16) glm::vec4 pos;
//	alignas(16) glm::vec4 dir;
//	alignas(16) glm::vec4 color;
//};

// light struct for vulkan aligment
struct light_t {
	//int enable;
	//int type;
	//float attenuation;
	//float cutOff;
	alignas(16) glm::vec4 data;
	alignas(16) glm::vec4 pos;
	alignas(16) glm::vec4 dir;
	alignas(16) glm::vec4 color;
};

class Light :
	public Entity
{
private:
	lightType_e type;
	glm::vec4 color;
	float attenuation;
	float cutOffAngle;
	bool enabled = true;

public:
	inline static const int N_LIGHTS = 8;
	inline static const int LIGHT_STRIDE = 7;

	lightType_e getType() const;
	void setType(lightType_e type);

	const glm::vec4& getColor() const;
	void setColor(const glm::vec4& color);

	virtual void step(float deltaTime) override {};// se implementa en una subclase heredera de Light

	//opcionales
	float getLinearAttenuation() const;
	void setLinearAttenuation(float att);
	bool getEnabled();
	void setEnable(bool enable);

	float getCutOffAngle() const;
	void setCutOffAngle(float cutoff);

	glm::vec3 getDir();

	void lookAt(glm::vec3 lookAt);
};

