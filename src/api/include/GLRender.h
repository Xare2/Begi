#pragma once
#include "common.h"
#include "Render.h"
#include "Object3D.h"
#include "GLTextureFB.h"

struct bufferObjectGL_t
{
	unsigned int bufferId; // Define un espacio en el que se reservan dos arrays
	unsigned int vertexBufferId;
	unsigned int indexBufferId;
};


class GLRender : public Render
{
protected:
	GLFWwindow *window = nullptr;
	std::map<int, bufferObjectGL_t> bufferObjectList;
	std::vector<frameBufferObject_t> frameBufferObjectList;

	std::map<std::string, GLTexture *> buffers;
	renderModes_e mode = onScreen;

public:
	GLRender();
	~GLRender();

	void init();
	void initGL();
	void initGLFW();
	void setupObject(Object *obj) override;
	void updateObject(Object *obj) override;
	void removeObject(Object *obj) override;
	void drawObject(Object *obj) override;
	void drawObjects(std::map<float, Object *> *objs) override;

	void setCurrentRenderStep(int currentStep) override;

	void toggleCursor(bool value) override;

	virtual void drawGL(Object *obj);

	virtual void swapBuffer() override;
	virtual bool isClosed() override;

	GLFWwindow *getWindow() override;

	void initFrameBuffer();
	void initFrameBuffers(int stepCount) override;
	void setupFrameBuffer() override;
	void setOutBuffer(std::string type, std::string bufferName) override;
	void setOutBuffer(int step, Texture::textureType_e type, GLTexture *tex) override;
	Texture *getBuffer(std::string bufferName) const override;
	Texture *getBuffer(int step, Texture::textureType_e type) override;
};