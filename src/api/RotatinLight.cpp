#include "RotatinLight.h"
#include "System.h"

void RotatinLight::step(float deltaTime)
{
	float s = sin(speed * deltaTime);
	float c = cos(speed * deltaTime);
	// translate point back to origin:

	float x0 = this->getPos().x - this->rotateAround.x;
	float z0 = this->getPos().z - this->rotateAround.z;

	// rotate point
	float xnew = x0 * c - z0 * s;
	float znew = x0 * s + z0 * c;

	// translate point back:
	this->setPos({
		xnew + rotateAround.x,
		this->getPos().y,
		znew + rotateAround.z

	});
}

void FollowCamLight::step(float deltaTime)
{
	Camera* cam = System::getWorld()->getCamera(System::getWorld()->getActiveCamera());

	this->setPos(cam->getPos());
}
