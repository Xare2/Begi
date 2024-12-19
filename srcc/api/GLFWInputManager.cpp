#include "GLFWInputManager.h"

void GLFWInputManager::keyboardManager(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GLFWInputManager* inputManager = reinterpret_cast<GLFWInputManager*>(glfwGetWindowUserPointer(window));
	switch (action) {
	case GLFW_PRESS:
	{
		inputManager->keyState[key] = true;
	}
	break;
	case GLFW_REPEAT:
	{
		;
	}
	break;
	case GLFW_RELEASE:
	{
		inputManager->keyState[key] = false;
	}
	break;
	}
}

void GLFWInputManager::mousePosManager(GLFWwindow* window, double xpos, double ypos)
{
	GLFWInputManager* inputManager = reinterpret_cast<GLFWInputManager*>(glfwGetWindowUserPointer(window));
	inputManager->mousePosKey.x = xpos;
	inputManager->mousePosKey.y = ypos;

	//std::cout << "pos x updated to " << inputManager->mousePosKey.x << std::endl;
}

void GLFWInputManager::mouseButtonManager(GLFWwindow* window, int button, int action, int mods)
{
	GLFWInputManager* inputManager = reinterpret_cast<GLFWInputManager*>(glfwGetWindowUserPointer(window));
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		if (action == GLFW_PRESS)
			inputManager->mousePosKey.buttonLeft = true;
		else
			inputManager->mousePosKey.buttonLeft = false;
	}
	break;
	case GLFW_MOUSE_BUTTON_RIGHT:
	{
		if (action == GLFW_PRESS)
			inputManager->mousePosKey.buttonRight = true;
		else
			inputManager->mousePosKey.buttonRight = false;
	}
	break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
	{
		if (action == GLFW_PRESS)
			inputManager->mousePosKey.buttonCentral = true;
		else
			inputManager->mousePosKey.buttonCentral = false;
	}
	break;
	}
}

void GLFWInputManager::init(GLFWwindow* window)
{
	//Codigo grafico opengl
	/*
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	*/

	// https://discourse.glfw.org/t/what-is-a-possible-use-of-glfwgetwindowuserpointer/1294
	glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this)); 

	glfwSetKeyCallback(window, keyboardManager);
	glfwSetCursorPosCallback(window, mousePosManager);
	glfwSetMouseButtonCallback(window, mouseButtonManager);
}
