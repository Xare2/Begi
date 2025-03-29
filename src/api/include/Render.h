#pragma once
#include "common.h"
#include "Object.h"


struct frameBufferObject_t
{
	unsigned int backBufferId = 0;
	GLTexture *colorBuffer;
	GLTexture *depthBuffer;
};

class Render
{
private:
	unsigned int width = 1280;
	unsigned int height = 720;

protected:
	int currentStep = 0;

public:
	typedef enum renderModes_e
	{
		offScreen,
		onScreen
	} renderModes_e;

	virtual void init() = 0;
	virtual void setupObject(Object *ojb) = 0;
	virtual void updateObject(Object *ojb) = 0;
	virtual void removeObject(Object *obj) = 0;
	virtual void drawObject(Object *obj) = 0;
	virtual void drawObjects(std::map<float, Object *> *objs) = 0;

	virtual GLFWwindow *getWindow() = 0;

	unsigned int getWidth();
	unsigned int getHeight();

	void setWidth(unsigned int width);
	void setHeight(unsigned int height);

	virtual bool isClosed() = 0;

	virtual void toggleCursor(bool value) = 0;

	virtual void swapBuffer() = 0;

	virtual void setCurrentRenderStep(int currentStep) = 0;
	virtual void initFrameBuffers(int stepCount) = 0;
	virtual void setupFrameBuffer() = 0;
	virtual void setOutBuffer(std::string type, std::string bufferName) = 0;
	virtual void setOutBuffer(int step, Texture::textureType_e type, GLTexture *tex) = 0;
	virtual Texture *getBuffer(std::string bufferName) const = 0;
	virtual Texture *getBuffer(int step, Texture::textureType_e type) = 0;
};