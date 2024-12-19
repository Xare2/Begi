#pragma once
#include "common.h"

class EventManager
{
public:
	static inline std::map<int, bool> keys; // Map es muy similar a un diccionario
	static inline GLFWwindow* window = nullptr;
	static inline struct {
		double x;
		double y;
		bool buttonLeft, buttonCentral, buttonRight;
	} mousePosKey;

	static void init(GLFWwindow* window) {
		EventManager::window = window;
		glfwSetKeyCallback(window, keyboardManager);
		glfwSetCursorPosCallback(window, mousePosManager);
		glfwSetMouseButtonCallback(window, mouseButtonManager);
	}

	static void updateEvents() {

		glfwPollEvents(); // Procesa los eventos si hay alguno pendiente
	}

	static void keyboardManager(GLFWwindow* window, int key, int scancode,
		int action, int mods)
	{
		switch (action) {
		case GLFW_PRESS:
		{
			keys[key] = true;
		}
		break;
		case GLFW_REPEAT:
		{
			;
		}
		break;
		case GLFW_RELEASE:
		{
			keys[key] = false;
		}
		break;
		}
	}

	static void mousePosManager(GLFWwindow* window, double xpos, double ypos)
	{
		EventManager::mousePosKey.x = xpos;
		EventManager::mousePosKey.y = ypos;
	}

	static void mouseButtonManager(GLFWwindow* window, int button, int action, int mods)
	{
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
		{
			if (action == GLFW_PRESS)
				mousePosKey.buttonLeft = true;
			else
				mousePosKey.buttonLeft = false;
		}
		break;
		case GLFW_MOUSE_BUTTON_RIGHT:
		{
			if (action == GLFW_PRESS)
				mousePosKey.buttonRight = true;
			else
				mousePosKey.buttonRight = false;
		}
		break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
		{
			if (action == GLFW_PRESS)
				mousePosKey.buttonCentral = true;
			else
				mousePosKey.buttonCentral = false;
		}
		break;
		}
	}

};