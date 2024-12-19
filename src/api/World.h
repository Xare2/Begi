#pragma once

#include<list>

#include "Object.h"
#include "Emitter.h"
#include "Camera.h"
#include "Light.h"

class World
{
private:
	int activeCamera = 0;

	std::list<Camera*> cameras;
	std::list<Object*> objects;
	std::list<Emitter*> emitters;

	glm::vec3 ambientColor;
	std::vector<Light*>* lights;

public:
	World();
	~World();

	void addObject(Object* obj);
	void removeObject(Object* obj);
	size_t getNumObjects() const;
	Object* getObject(size_t index) const;

	void addEmitter(Emitter* emt);
	void removeEmitter(Emitter* emt);
	size_t getNumEmiters() const;
	Emitter* getEmitter(size_t index) const;

	void addCamera(Camera* cam);
	void removeCamera(Camera* cam);
	Camera* getCamera() const;
	Camera* getCamera(size_t index) const;

	int getActiveCamera();
	void setActiveCamera(int cameraID);

	std::list<Object*>& getObjects();
	std::list<Emitter*> getEmitters();
	void update(float deltaTime);

	glm::vec3 getAmbientColor() const;
	void setAmbientColor(const glm::vec3& ambient);

	std::vector<Light*>* getLights();
	void addLight(Light* light);
	void deleteLight(int lightPos);
};

