#include "Light.h"

lightType_e Light::getType() const
{
	return this->type;
}

void Light::setType(lightType_e type)
{
	this->type = type;
}

const glm::vec4& Light::getColor() const
{
	return color;
}

void Light::setColor(const glm::vec4& color)
{
	this->color = color;
}

float Light::getLinearAttenuation() const
{
	return this->attenuation;
}

void Light::setLinearAttenuation(float att)
{
	this->attenuation = att;
}

bool Light::getEnabled()
{
	return this->enabled;
}

void Light::setEnable(bool enable)
{
	this->enabled = enable;
}

float Light::getCutOffAngle() const
{
	return this->cutOffAngle;
}

void Light::setCutOffAngle(float cutoff)
{
	this->cutOffAngle = cutoff;
}

// pitch x
// yaw y
// roll z
glm::vec3 Light::getDir()
{
	glm::vec3 dir = glm::vec3(0);

	dir.x = -glm::cos(this->getRot().x) * glm::sin(this->getRot().y);
	dir.y = glm::sin(this->getRot().x);
	dir.z = -glm::cos(this->getRot().x) * glm::cos(this->getRot().y);

	dir = glm::normalize(dir);

	return dir;
}

void Light::lookAt(glm::vec3 lookAt)
{
	//glm::quat lookAtQ = glm::quatLookAt(glm::normalize(this->getPos() - lookAt), { 0, 1, 0 });
	glm::mat3 rmat = glm::mat3(
		glm::transpose(glm::lookAt(this->getPos(), lookAt, { 0, 1, 0 }))
	);
	this->setRot(glm::eulerAngles(glm::quat_cast(rmat)));
}
