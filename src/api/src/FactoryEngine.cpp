#include "FactoryEngine.h"

#include <stdexcept>
#include "common.h"
#include "VKRender.h"
#include "GLRender.h"
#include "GL4Render.h"
#include "GLFWInputManager.h"
#include "GLTexture.h"
#include "VKTexture.h"

inputBackend_e FactoryEngine::getInputBackend()
{
	return selectedInputBackend;
}

graphicsBackend_e FactoryEngine::getGraphicsBackend()
{
	return selectedGraphicsBackend;
}

void FactoryEngine::setInputBackend(inputBackend_e inputBackend)
{
	FactoryEngine::selectedInputBackend = inputBackend;
}

void FactoryEngine::setGraphicsBackend(graphicsBackend_e graphicsBackend)
{
	FactoryEngine::selectedGraphicsBackend = graphicsBackend;
}

Render* FactoryEngine::getNewRender()
{
	switch (selectedGraphicsBackend)
	{
	case OpenGL1_0:
		render = new GLRender();
		break;
	case OpenGL4_0:
		render = new GL4Render();
		break;
	case Vulkan:
		render = new VKRender();
		break;
	default:
		render = new GLRender();
		break;
	}

	// return opengl as default
	return render;
}

InputManager* FactoryEngine::getNewInputManager()
{
	switch (selectedInputBackend)
	{
	case GLFW:
		return new GLFWInputManager();
	}

	return new GLFWInputManager();
}

Texture* FactoryEngine::getNewTexture()
{
	if (selectedGraphicsBackend == Vulkan)
		return new VKTexture();

	return new GLTexture();
}

Material* FactoryEngine::getNewMaterial()
{
	if (selectedGraphicsBackend == OpenGL4_0)
		return new GLSLMaterial();

	if (selectedGraphicsBackend == Vulkan)
		return new VKMaterial();

	return nullptr;
}

bool FactoryEngine::isClosed()
{
	return glfwWindowShouldClose(render->getWindow());
}
