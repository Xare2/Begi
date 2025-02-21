#pragma once

#include <map>
#include "common.h"
class InputManager
{
protected:
	std::map<int, bool> keyState;

public:

	struct mousePosKey_t {
		double x;
		double y;
		bool buttonLeft, buttonCentral, buttonRight;
	};
	mousePosKey_t mousePosKey{};


	virtual void init(GLFWwindow* window) = 0;
	bool isPressed(char key);
	bool isPressed(int key);

	bool justPressed(char key);
};

