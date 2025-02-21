#include "World.h"

World::World()
{
	this->objects = std::list<Object*>();
	this->lights = new std::vector<Light*>();

	this->ambientColor = { 0.1f, .1f, .1f };
}

World::~World()
{
	//while (!this->objects.empty())
	//{
	//	delete this->objects.front();
	//	this->objects.pop_front();
	//}
}

void World::addObject(Object* obj)
{
	this->objects.push_back(obj);
}

void World::removeObject(Object* obj)
{
	this->objects.remove(obj);
}

size_t World::getNumObjects() const
{
	return this->objects.size();
}

Object* World::getObject(size_t index) const
{
	std::list<Object*>::const_iterator it = this->objects.begin();
	for (size_t i = 0; i < index; i++)
		it++;
	return *it;
}

void World::addEmitter(Emitter* emt)
{
	this->emitters.push_back(emt);
}

void World::removeEmitter(Emitter* emt)
{
	this->emitters.remove(emt);
}

size_t World::getNumEmiters() const
{
	return this->emitters.size();
}

Emitter* World::getEmitter(size_t index) const
{
	std::list<Emitter*>::const_iterator it = this->emitters.begin();
	for (size_t i = 0; i < index; i++)
		it++;
	return *it;
}

void World::addCamera(Camera* cam)
{
	this->cameras.push_back(cam);
}

void World::removeCamera(Camera* cam)
{
	this->cameras.remove(cam);
}

Camera* World::getCamera() const
{
	return this->getCamera(this->activeCamera);
}

Camera* World::getCamera(size_t index) const
{
	if (index < 0 || index >= this->cameras.size())
	{
		// throw new std::exception("Camera index out of range");
		return nullptr;
	}

	std::list<Camera*>::const_iterator it = this->cameras.begin();
	for (size_t i = 0; i < index; i++)
		it++;
	return *it;
}

int World::getActiveCamera()
{
	return activeCamera;
}

void World::setActiveCamera(int cameraID)
{
	if (cameras.size() == 0) {
		std::cout << "No existe ninguna camara \n";
	}
	else if (cameraID < 0) {
		std::cout << "El ID de la camara seleccioanda no puede ser menor que 0 \n";
	}
	else if( cameraID > cameras.size()) {
		std::cout << "No existe una camara con ID " << cameraID << "\n";
	}
	else {
		activeCamera = cameraID;
	}
	
}

std::list<Object*>& World::getObjects()
{
	return this->objects;
}

std::list<Emitter*> World::getEmitters()
{
	return this->emitters;
}

void World::update(float deltaTime)
{
	for (auto& c : this->cameras)
	{
		c->step(deltaTime);
		c->computeViewMatrix();
		c->computeProjectionMatrix();
	}

	//Camera* camera = getCamera(getActiveCamera());
	//camera->step(deltaTime);
	//camera->computeViewMatrix();
	//camera->computeProjectionMatrix();

	for (auto& obj : objects)
	{
		obj->step(deltaTime);
	}

	for (auto& emt : emitters)
	{
		emt->step(deltaTime);
	}

	for (auto& l : *this->lights)
	{
		l->step(deltaTime);
	}
}

glm::vec3 World::getAmbientColor() const
{
	return ambientColor;
}

void World::setAmbientColor(const glm::vec3& ambient)
{
	this->ambientColor = ambient;
}

std::vector<Light*>* World::getLights()
{
	return this->lights;
}

void World::addLight(Light* light)
{
	if (this->lights->size() > Light::N_LIGHTS)
		return;
	this->lights->push_back(light);
}

void World::deleteLight(int lightPos)
{
	std::vector<Light*>::iterator it = this->lights->begin();

	it += lightPos;

	this->lights->erase(it);
}
