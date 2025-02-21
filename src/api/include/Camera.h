#pragma once
#include "Entity.h"
#include "Light.h"

class Camera : public Entity
{
private:
	glm::mat4 view = glm::mat4(0);
	glm::mat4 projection = glm::mat4(0);

public:
	inline static const float NEAR = .01f;
	inline static const float FAR = 1000.f;

	glm::vec3 up = { 0, 1, 0 }; // Vector por defecto para ser perpendicular al suelo
	glm::vec3 lookAt = { 0, 0, 0 };

	float speed = 2.0f;
	float baseSpeed = 2.0f;

	typedef enum {
		ortho, perspective
	}CameraType;

	CameraType type;

	Camera(CameraType projectionType, glm::vec3 position, glm::vec3 up, glm::vec3 lookAt);

	glm::mat4 getProjection();
	glm::mat4 getView();
	glm::mat4 getStaticView();

	void computeProjectionMatrix();
	void computeViewMatrix();

	virtual void step(float deltaTime) = 0;

};

class StaticCamera : public Camera {
public:
	StaticCamera(CameraType projectionType, glm::vec3 position, glm::vec3 up, glm::vec3 lookAt) :
		Camera(projectionType, position, up, lookAt) {}
	virtual void step(float deltaTime) override {}
};

class CameraKeyboard : public Camera {
public:
	static inline double oldX = 0;
	static inline double oldY = 0;

	glm::vec3 direction, originalDirection, strafeDirection;
	glm::vec3 originalUp;
	float radius = 0;

	bool shouldUpdate = true;

	CameraKeyboard(glm::vec3 pos, glm::vec3 lookAt) : Camera(perspective, pos, { 0, 1, 0 }, lookAt) {
		direction = originalDirection = glm::normalize(pos - lookAt);
		originalUp = up;
		strafeDirection = glm::normalize(glm::cross(direction, up));
		radius = glm::distance(pos, lookAt);
	}

	virtual void step(float deltaTime) override;
};

class CloneCamera : public Camera {
public:
	Camera* toClone;

	CloneCamera(Camera* c) : Camera(c->type, c->getPos(), { 0, 1, 0 }, c->lookAt) {
		this->toClone = c;
	}

	virtual void step(float deltaTime) override;
};

class RotatingCamera : public Camera
{
private:
	glm::vec3 rotateAround;

public:
	RotatingCamera(glm::vec3 pos, glm::vec3 lookAt, float speed);

	virtual void step(float deltaTime) override;
};

class ShadowCaster : public Camera
{
private:
	Light* lightSource;

public:
	ShadowCaster();
	ShadowCaster(Light* l);

	virtual void step(float deltaTime) override;
};
