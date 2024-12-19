#pragma once
#include "common.h"
#include "Render.h"
#include "Object3D.h"
#include "GLTextureFB.h"

struct bufferObject_t {
	unsigned int bufferId; //Define un espacio en el que se reservan dos arrays
	unsigned int vertexBufferId;
	unsigned int indexBufferId;
};

struct frameBufferObject_t {
	unsigned int backBufferId = 0;
	GLTexture* colorBuffer;
	GLTexture* depthBuffer;
};

class GLRender : public Render
{
protected:
	GLFWwindow* window = nullptr;
	//GLTexture* colorBuffer = nullptr;
	//GLTexture* depthBuffer = nullptr;
	std::map<int, bufferObject_t> bufferObjectList;
	std::vector<frameBufferObject_t> frameBufferObjectList;

	std::map<std::string, GLTexture*> buffers;
	renderModes_e mode = onScreen;
	int currentStep = 0;

public:
	GLRender();
	~GLRender();

	void init();
	void initGL();
	void initGLFW();
	void setupObject(Object* obj) override;
	void updateObject(Object* obj) override;
	void removeObject(Object* obj) override;
	void drawObject(Object* obj) override;
	void drawObjects(std::list<Object*>* objs) override;
	void drawObjects(std::map<float, Object*>* objs) override;

	void setCurrentRenderStep(int currentStep);

	void toggleCursor(bool value) override;

	virtual void drawGL(Object* obj);

	virtual void swapBuffer() override;
	virtual bool isClosed() override;

	GLFWwindow* getWindow() override;

	void initFrameBuffers();
	void initFrameBuffers(int stepCount);
	void setupFrameBuffer();
	void setOutBuffer(std::string type, std::string bufferName);
	void setOutBuffer(int step, Texture::textureType_e type, GLTexture* tex);
	GLTexture* getBuffer(std::string bufferName) const;
	GLTexture* getBuffer(int step, Texture::textureType_e type);
};