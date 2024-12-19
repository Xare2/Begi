#pragma once

#include "Render.h"
#include "Material.h"
#include "InputManager.h"

enum graphicsBackend_e
{
	OpenGL1_0,
	OpenGL4_0,
	Vulkan
};

enum inputBackend_e
{
	GLFW
};

class FactoryEngine
{
private:
	static inline inputBackend_e selectedInputBackend = GLFW;
	static inline graphicsBackend_e selectedGraphicsBackend = OpenGL1_0;

	static inline Render* render = nullptr;

public:
	static inputBackend_e getInputBackend();
	static graphicsBackend_e getGraphicsBackend();

	static void setInputBackend(inputBackend_e inputBackend);
	static void setGraphicsBackend(graphicsBackend_e graphicsBackend);

	static Material* getNewMaterial();
	static Render* getNewRender();
	static InputManager* getNewInputManager();
	static Texture* getNewTexture();
	static bool isClosed();
};

