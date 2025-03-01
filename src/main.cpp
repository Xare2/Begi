/*
PRACTICAS PARA APIS TRIDIMENSIONALES REALIZADAS POR
XABIER GOENAGA URKIOLA

M�STER EN COMPUTACI�N GR�FICA, SIMULACI�N Y REALIDAD VIRTUAL U-TAD
A�O 2023-2024
*/

#define GLAD_GL_IMPLEMENTATION // Incluir esta cabecera SOLO en un archivo y antes del common.h

#include "api/include/common.h"
#include "api/include/System.h"
#include "api/include/GLRender.h"
#include "api/include/RotatinLight.h"
#include "api/include/FactoryEngine.h"
#include <filesystem>

void VolumeCloud();
void SceneCloud();

int main(int argc, char **argv)
{
	std::cout << "Hello from " << std::filesystem::current_path() << "!\n";
	// VolumeCloud();
	SceneCloud();

	return 0;
}

void VolumeCloud()
{
	FactoryEngine::setInputBackend(GLFW);
	FactoryEngine::setGraphicsBackend(OpenGL4_0);

	System::readPipeLine("./data/config_simple.pipeline");
	System::initSystem();

	World world = World();
	world.setAmbientColor({.1f, .1f, .1f});
	System::setWorld(&world);

	// FollowCamObject skybox = FollowCamObject("./data/skyboxObj.msh");
	// skybox.setSize(glm::vec3(50000));

	// Object3D scene = Object3D("./data/scene.msh");
	// scene.setPos(glm::vec3(0.0f, -2.0f, -10.0f));
	// scene.setSize(glm::vec3(5));

	// BoxCloud cloudBox = BoxCloud();
	// cloudBox.setSize(glm::vec3(10000.f, 4000.f - 1500.f, 10000.f));
	// cloudBox.setPos(glm::vec3(0.f, 4000.f, 0.f));

	Light light = Light();
	light.setColor({.95f, .55f, .025f, 1});
	light.setColor({.95f, .95f, .95f, 1});
	light.setType(lightType_e::directional);
	light.setPos({6, 6, 6});
	light.lookAt({0, 0, 0});

	// ShadowCaster lightCam = ShadowCaster(&light);
	CameraKeyboard kcam = CameraKeyboard({0, 0, 0}, {0, 0, 1});
	CloneCamera kcam2 = CloneCamera(&kcam);

	// System::addObject(0, &skybox);
	// System::addObject(0, &scene);
	// System::addObject(0, &cloudBox);

	// System::addObject(1, &skybox);
	// System::addObject(1, &scene);
	// System::addObject(1, &cloudBox);

	// System::addCamera(0, &lightCam);
	System::addCamera(1, &kcam);

	System::addLight(0, &light);
	System::addLight(1, &light);

	System::mainLoop();

	System::terminateSystem();
}

void SceneCloud()
{
	FactoryEngine::setInputBackend(GLFW);
	FactoryEngine::setGraphicsBackend(OpenGL4_0);

	System::readPipeLine("./data/config.pipeline");
	System::initSystem();

	World world = World();
	world.setAmbientColor({.1f, .1f, .1f});
	System::setWorld(&world);

	// FollowCamObject skybox = FollowCamObject("./data/skyboxObj.msh");
	// skybox.setSize(glm::vec3(500.f));

	Object3D scene = Object3D("./data/terrain.msh");
	scene.setPos(glm::vec3(-500.f, -25.0f, 500.0f));

	Light light = Light();
	light.setColor({.9f, .85f, .95f, 1});
	// light.setColor(glm::vec4({ 224, 167, 162 ,255 }) / 255.f);
	light.setType(lightType_e::directional);
	light.setPos({0, 0, 10});
	light.setRot({4.5, 0, 0});

	ShadowCaster lightCam = ShadowCaster(&light);

	CameraKeyboard kcam = CameraKeyboard({0, 0, 0}, {0, 0, 1});
	// kcam.setPos({ 0,3000,0 });

	CloneCamera kcam2 = CloneCamera(&kcam);

	PostProcessPlane clouds = PostProcessPlane(1, &kcam);

	System::addObject(0, &scene);

	System::addObject(1, &scene);

	System::addObject(2, &clouds);

	System::addCamera(0, &lightCam);
	System::addCamera(1, &kcam);
	System::addCamera(2, &kcam2);

	System::addLight(0, &light);
	System::addLight(1, &light);

	System::mainLoop();

	System::terminateSystem();
}
