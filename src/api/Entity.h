#pragma once
#include "common.h"

class Entity
{
private:
	glm::vec3 pos = { 0, 0, 0 };
	glm::vec3 rot = { 0, 0, 0 };
	glm::vec3 size = { 1.0f, 1.0f, 1.0f };
	glm::mat4 modelM;

public:
	virtual void computeModelMatrix();

	virtual void step(float deltaTime) = 0;

	glm::vec3 getPos();
	glm::vec3 getRot();
	glm::vec3 getSize();
	glm::mat4 getModelMatrix();

	void setPos(glm::vec3 _pos);
	void setRot(glm::vec3 _rot);
	void setSize(glm::vec3 _size);
	void setModelMatrix(glm::mat4 _model);
	
};

