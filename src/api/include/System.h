#pragma once

#include "World.h"
#include "Render.h"
#include "Object3D.h"
#include "Camera.h"
#include "InputManager.h"
#include "FactoryEngine.h"

#include "common.h"

class PipelineNode 
{
public:
	int nodeIdx = 0;
	//<tipo, nombre>
	std::map<std::string, std::string> input;
	//<tipo, nombre>
	std::map<std::string, std::string> output;
};


class System
{
private:
	static inline World* world = nullptr;
	static inline Render* render = nullptr;
	static inline InputManager* inputManager = nullptr;
	static inline Object* activeObject = nullptr;
	static inline glm::mat4 modelMatrix = glm::mat4();
	static inline bool end = false;

	static inline std::map<int, std::vector<Object3D*>> objectList;
	static inline std::map<int, std::vector<Camera*>> cameras;
	static inline std::map<int, std::vector<Light*>> lights;
	static inline std::map<int, std::vector<Emitter*>> emitters;

	static inline std::map<void*, Entity*> uniqueEntities;

	static inline int stepNumber = 0;
	static inline std::vector<PipelineNode> pipeline;

	static inline float fps;
	static inline float time;
	static inline float deltaTime;
	static inline float stepDeltaTime;

public:
	static void exit();
	static void mainLoop();
	static void initSystem();
	static void terminateSystem();
	static void addObject(Object* obj);

	static Render* getRender();

	static World* getWorld();
	static void setWorld(World* world);

	static Object* getActiveObject();
	static void setActiveObject(Object* object);

	static glm::mat4 getModelMatrix();
	static void setModelMatrix(glm::mat4 modelMatrix);

	static InputManager* getInputManager();

	static bool shouldExit();
	static int getStepCount();

	static void readPipeLine(std::string fileName);
	static void addLight(int pipelineStep, Light* l);
	static void addCamera(int pipelineStep, Camera* c);
	static void addEmitter(int pipelineStep, Emitter* l);
	static void addObject(int pipelineStep, Object3D* obj);

	static void drawGUI();

	static float getTime();
};