#pragma once
#include "Light.h"
class RotatinLight :
	public Light
{
private:
	glm::vec3 rotateAround;
	float speed = 1.f;

public:
	void step(float deltaTime) override;
};

class FollowCamLight : public Light
{
public:
	void step(float deltaTime) override;
};
