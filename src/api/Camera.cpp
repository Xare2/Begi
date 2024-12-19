#include "Camera.h"
#include "System.h"

Camera::Camera(CameraType projectionType, glm::vec3 position, glm::vec3 up, glm::vec3 lookAt)
{
	this->type = projectionType;
	this->setPos(position);
	this->up = up;
	this->lookAt = lookAt;

	this->setRot({ 0, M_PI, 0 });
}

glm::mat4 Camera::getProjection()
{
	return projection;
}

glm::mat4 Camera::getView()
{
	return view;
}

glm::mat4 Camera::getStaticView()
{
	return glm::lookAt({0, 0, 0}, lookAt, up);
}

void Camera::computeProjectionMatrix()
{
	const float ORTHO_SIZE = 10.f;
	switch (type)
	{
	case ortho:
		projection = glm::ortho(-ORTHO_SIZE, ORTHO_SIZE,
			-ORTHO_SIZE, ORTHO_SIZE, 0.01f, 1000.0f);
		break;

	case perspective:
		float aspect = (float)System::getRender()->getWidth() / (float)System::getRender()->getHeight();
		if (FactoryEngine::getGraphicsBackend() == Vulkan)
		{
			projection = glm::perspectiveZO(glm::radians(90.0f), aspect, NEAR, FAR);
		}
		else
		{
			projection = glm::perspective(glm::radians(90.0f), aspect, NEAR, FAR);
			break;
		}
	}
}

void Camera::computeViewMatrix()
{
	view = glm::lookAt(getPos(), lookAt, up);
}

void CameraKeyboard::step(float deltaTime)
{
	InputManager* inputManager = System::getInputManager();
	glm::vec3 pos = this->getPos();
	glm::vec3 rot = this->getRot();

	if (inputManager->isPressed('A')) {
		pos += speed * deltaTime * strafeDirection;
		lookAt += speed * deltaTime * strafeDirection;
	}
	if (inputManager->isPressed('D')) {
		pos -= speed * deltaTime * strafeDirection;
		lookAt -= speed * deltaTime * strafeDirection;
	}
	if (inputManager->isPressed('S')) {
		pos += speed * deltaTime * direction;
		lookAt += speed * deltaTime * direction;
	}
	if (inputManager->isPressed('W')) {
		pos -= speed * deltaTime * direction;
		lookAt -= speed * deltaTime * direction;
	}
	if (inputManager->isPressed(GLFW_KEY_LEFT_SHIFT)) {
		speed = baseSpeed * 10;
	}
	else if (inputManager->isPressed(GLFW_KEY_LEFT_CONTROL)) {
		speed = baseSpeed / 2;
	}
	else {
		speed = baseSpeed;
	}

	if (inputManager->justPressed('L')) {
		this->shouldUpdate = !this->shouldUpdate;
		System::getRender()->toggleCursor(this->shouldUpdate);
	}

	if (!this->shouldUpdate)
		return;

	this->setPos(pos);

	// matriz de rotacion en x/y

	float velX = (float)(oldX - inputManager->mousePosKey.x);
	float velY = (float)(oldY - inputManager->mousePosKey.y);

	float delta = 0.01f;
	rot.y += velX * delta;
	rot.x -= velY * delta;

	if (glm::abs(rot.x) > glm::radians(55.0f)) {
		rot.x += velY * delta;
	}

	glm::mat4 mrot = glm::rotate(glm::mat4(1.0f), rot.y, { 0.f, 1.0f, 0.f });
	mrot = glm::rotate(mrot, rot.x, { 1.0f, 0.f, 0.f });

	lookAt = radius * originalDirection;
	lookAt = mrot * glm::vec4(lookAt, 1.0f);
	lookAt += pos;

	up = glm::inverse(glm::transpose(mrot)) * glm::vec4(originalUp, 0.f);
	direction = glm::normalize(pos - lookAt);
	strafeDirection = glm::normalize(glm::cross(direction, up));

	oldX = inputManager->mousePosKey.x;
	oldY = inputManager->mousePosKey.y;

	this->setRot(rot);
}

RotatingCamera::RotatingCamera(glm::vec3 pos, glm::vec3 lookAt, float speed) : Camera(perspective, pos, { 0, 1, 0 }, lookAt)
{
	this->speed = speed;
	this->rotateAround = lookAt;
}

void RotatingCamera::step(float deltaTime)
{
	glm::vec3 pos = this->getPos();

	float step = speed * deltaTime;

	float s = glm::sin(step);
	float c = glm::cos(step);

	float px = pos.x - rotateAround.x;
	float pz = pos.z - rotateAround.z;

	float pnx = px * c - pz * s;
	float pnz = px * s + pz * c;

	this->setPos({
		pnx,
		pos.y,
		pnz
		});


	glm::vec3 dir = glm::normalize(this->getPos() - this->lookAt);

	this->setRot(glm::eulerAngles(glm::quatLookAt(dir, this->up)));
}

ShadowCaster::ShadowCaster() : Camera(CameraType::perspective, { 0, 0, 1 }, { 0, 1, 0 }, { 0, 0, 0 })
{
	this->lightSource = nullptr;
}

ShadowCaster::ShadowCaster(Light* l) : Camera(CameraType::ortho, l->getPos(), {0, 1, 0}, {0, 0, 0})
{
	this->lightSource = l;
}

void ShadowCaster::step(float deltaTime)
{
	this->setPos(this->lightSource->getPos());
	this->setRot(this->lightSource->getRot());
}

void CloneCamera::step(float deltaTime)
{
	this->setPos(this->toClone->getPos());
	this->setRot(this->toClone->getRot());
}
