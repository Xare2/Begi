#include "InputManager.h"

bool InputManager::isPressed(char key)
{	
	// GLFW_KEY_ = ASCI VALUE
	return keyState[key];
}

bool InputManager::isPressed(int key)
{
	// GLFW_KEY_ = ASCI VALUE
	return keyState[key];
}

bool InputManager::justPressed(char key)
{
	bool pressed = keyState[key];

	if (pressed)
		keyState[key] = false;

	return pressed;
}
