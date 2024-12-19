#pragma once
#include "InputManager.h"
#include "common.h"

class GLFWInputManager : public virtual InputManager
{
private:
    static void keyboardManager(GLFWwindow* window, int key, int scancode,
        int action, int mods);

	static void mousePosManager(GLFWwindow* window, double xpos, double ypos);
	
	static void mouseButtonManager(GLFWwindow* window, int button, int action, int mods);
	
public:

    void init(GLFWwindow* window) override;
};

