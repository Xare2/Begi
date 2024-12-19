#pragma once
#include "common.h"
#include "Object.h"

class Render
{
private:
	unsigned int width = 1280;
	unsigned int height = 720;

public:
	typedef enum renderModes_e {
		offScreen, onScreen
	} renderModes_e;

	virtual void init() = 0;
	virtual void setupObject(Object* ojb) = 0;
	virtual void updateObject(Object* ojb) = 0;
	virtual void removeObject(Object* obj) = 0;
	virtual void drawObject(Object* obj) = 0;
	virtual void drawObjects(std::list<Object*>* objs) = 0;
	virtual void drawObjects(std::map<float, Object*>* objs) = 0;

	virtual GLFWwindow* getWindow() = 0;

	unsigned int getWidth();
	unsigned int getHeight();

	void setWidth(unsigned int width);
	void setHeight(unsigned int height);

	virtual bool isClosed() = 0;

	virtual void toggleCursor(bool value) = 0; 

	virtual void swapBuffer() = 0;
};