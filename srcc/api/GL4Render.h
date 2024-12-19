#pragma once
#include "common.h"
#include "GLRender.h"
#include "Object3D.h"

class GL4Render : public GLRender 
{
private:

public:
	GL4Render() {};
	GL4Render(int width, int height) 
	{
		window = nullptr;
	}
	~GL4Render() 
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	//GL4RENDER
	void drawGL(Object* obj) override;
	void setupObject(Object* obj) override;
	void updateObject(Object* obj) override;
	void removeObject(Object* obj) override;

};