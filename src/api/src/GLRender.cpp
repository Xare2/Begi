#include "GLRender.h"
#include "System.h"

GLRender::GLRender() : Render()
{
	this->bufferObjectList = std::map<int, bufferObject_t>();

	//initGLFW();
	//initGL();

	setWidth(1280);
	setHeight(720);
}

void GLRender::initGLFW()
{
	int error = glfwInit();

	//Codigo grafico opengl
	/*
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	*/

	//iniciar ventana
	window = glfwCreateWindow(getWidth(), getHeight(), "APIS3D 2024",
		nullptr, nullptr);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Quitamos la opcion de redimension de la ventana
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, 0.0, 0.0);

	glfwMakeContextCurrent(window);

	// Se carga OpenGL despues del contexto de la ventana
	gladLoadGL(glfwGetProcAddress);
}



void GLRender::init()
{
	initGLFW();
	initGL();
	initFrameBuffers(System::getStepCount());
}

void GLRender::initGL()
{
	//activar opciones
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClearColor(.1f, .1f, .1f, 1.f);
}

void GLRender::drawGL(Object* obj)
{
	//std::vector<unsigned int>& idList = obj.idList;
	obj->computeModelMatrix();

	System::setModelMatrix(obj->getModelMatrix());
	System::setActiveObject(obj);

	for (auto& mesh : obj->getMeshes()){

		std::vector<vertex_t>* vertexList = mesh->getVertList();

		glm::vec4& color = mesh->colorRGBA;

		glBegin(GL_TRIANGLES);

		for (size_t i = 0; i < vertexList->size(); i++)
		{
			glm::vec4 vertex = System::getModelMatrix() * vertexList->at(i).pos;
			glm::vec4 vertexColor = vertexList->at(i).color;
	
			glVertex3f(vertex.x, vertex.y, vertex.z);

			glColor4f(color.r * vertexColor.r, color.g * vertexColor.g, color.b * vertexColor.b, color.a);
		}

		glEnd();
	} 
}

void GLRender::setupObject(Object* ojb)
{
	for (auto& mesh : ojb->getMeshes()) 
	{
		if (bufferObjectList.count(mesh->getMeshID()))
			continue;

		bufferObject_t buffer;
		//Reservamos identificadores
		glGenVertexArrays(1, &buffer.bufferId);
		glGenBuffers(1, &buffer.vertexBufferId);
		glGenBuffers(1, &buffer.indexBufferId);

		////Copiamos datos de vertices
		glBindBuffer(GL_ARRAY_BUFFER, buffer.vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * mesh->getVertList()->size(),
			mesh->getVertList()->data(), GL_STATIC_DRAW);

		bufferObjectList[mesh->getMeshID()] = buffer;
	}
}

void GLRender::updateObject(Object* obj)
{
}

void GLRender::removeObject(Object* obj)
{
}

void GLRender::drawObject(Object* obj)
{
	this->drawGL(obj);
}

void GLRender::drawObjects(std::list<Object*>* objs)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto it = objs->begin();

	while (it != objs->end())
	{
		drawGL((Object3D*)*it);
		it++;
	}

	glDepthMask(GL_TRUE);

	//this->swapBuffer();
}

void GLRender::drawObjects(std::map<float, Object*>* objs)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto it = objs->begin();

	while (it != objs->end())
	{
		drawGL((Object3D*)it->second);
		it++;
	}

	glDepthMask(GL_TRUE);

	//this->swapBuffer();
}

void GLRender::setCurrentRenderStep(int currentStep)
{
	this->currentStep = currentStep;
}

void GLRender::toggleCursor(bool value)
{
	auto val = value ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
	glfwSetInputMode(window, GLFW_CURSOR, val);
	//glfwMakeContextCurrent(window);
}

bool GLRender::isClosed()
{
	return glfwWindowShouldClose(window);
}

GLFWwindow* GLRender::getWindow()
{
	return window;
}

void GLRender::swapBuffer()
{
	glfwSwapBuffers(window);
}

GLRender::~GLRender()
{
	glfwDestroyWindow(window);
	//borrar buffers de objects de gpu
	glfwTerminate();
}

void GLRender::initFrameBuffers()
{
	frameBufferObject_t fbo{};
	fbo.colorBuffer = new GLTextureFB(
		GLTexture::textureType_e::colorBuffer, this->getWidth(), this->getHeight()
	);

	fbo.depthBuffer = new GLTextureFB(
		GLTexture::textureType_e::depthBuffer, this->getWidth(), this->getHeight()
	);

	glGenFramebuffers(1, &fbo.backBufferId); //crear el identificador de frameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.backBufferId); //asociar la textura anterior a ese nuevo framebuffer

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.colorBuffer->getTexId(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo.depthBuffer->getTexId(), 0);

	glDrawBuffer(GL_NONE); //no se generar�n datos de pantalla
	glReadBuffer(GL_NONE);

	//volver al principal
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//check errores
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR" << std::endl;
		return;
	}

	this->frameBufferObjectList.push_back(fbo);
}

void GLRender::initFrameBuffers(int stepCount)
{
	for (int i = 0; i < stepCount; i++)
	{
		this->initFrameBuffers();
	}
}

void GLRender::setupFrameBuffer()
{
	switch (this->mode)
	{
	case offScreen:
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferObjectList[this->currentStep].backBufferId);
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLenum drawBuf = GL_COLOR_ATTACHMENT0;
		glDrawBuffers(1, &drawBuf);
	}
	break;

	case onScreen:
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	break;
	}
}

void GLRender::setOutBuffer(std::string type, std::string bufferName)
{
	mode = offScreen;
	if (type == "color")	//guardar los buffers de salida con un nuevo nombre
	{
		buffers[bufferName] = this->frameBufferObjectList[this->currentStep].colorBuffer;
	}
	else if (type == "depth")
	{
		buffers[bufferName] = this->frameBufferObjectList[this->currentStep].depthBuffer;
	}
	else if (type == "screen")	//si se activa el modo pantalla
	{
		mode = onScreen;
	}
}

void GLRender::setOutBuffer(int step, Texture::textureType_e type, GLTexture* tex)
{
	frameBufferObject_t fbo = this->frameBufferObjectList[step];
	switch (type)
	{
	case Texture::cubic:
	case Texture::color2D:
	case Texture::colorBuffer:
		fbo.colorBuffer = tex;
		break;
	case Texture::depthBuffer:
		fbo.depthBuffer = tex;
		break;
	default:
		break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.backBufferId); //asociar la textura anterior a ese nuevo framebuffer

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.colorBuffer->getTexId(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo.depthBuffer->getTexId(), 0);

	glDrawBuffer(GL_NONE); //no se generar�n datos de pantalla
	glReadBuffer(GL_NONE);

	//volver al principal
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//check errores
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR" << std::endl;
		return;
	}

	this->frameBufferObjectList[step] = fbo;
}

GLTexture* GLRender::getBuffer(std::string bufferName) const
{
	//return buffers[bufferName];
	return buffers.at(bufferName);
}

GLTexture* GLRender::getBuffer(int step, Texture::textureType_e type)
{
	switch (type)
	{
	case Texture::cubic:
	case Texture::color2D:
	case Texture::colorBuffer:
		return this->frameBufferObjectList[step].colorBuffer ;
		break;
	case Texture::depthBuffer:
		return this->frameBufferObjectList[step].depthBuffer ;
		break;
	}
	return nullptr;
}
