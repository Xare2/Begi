#include "System.h"

#include "GL4Render.h"
#include "CloudConfiguration.h"

void System::exit()
{
	end = true;
}

void System::mainLoop()
{
	//LLamar a la funci�n �setupObject� de render con cada objeto del mundo que est� activo
	for (auto& obj : world->getObjects())
	{
		render->setupObject(obj);
	}

	float newTime = 0;
	//float deltaTime = 0;
	float lastTime = static_cast<float>(glfwGetTime());;

	while (!end && !FactoryEngine::isClosed())
	{
		newTime = static_cast<float>(glfwGetTime());
		deltaTime = newTime - lastTime;
		lastTime = newTime;

		fps = 1.f / deltaTime;

		time = lastTime;

		world->update(deltaTime);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow(); // Show demo window! :)

		float stepBegin, stepEnd;

		int stepNumber = 0;
		for (auto& step : pipeline)
		{
			((GLRender*)render)->setCurrentRenderStep(stepNumber);
			world->setActiveCamera(stepNumber);
			Camera* activeCamera = world->getCamera();
			//Camera* activeCamera = System::cameras.at(stepNumber)[0];

			std::map<float, Object*> orderedObjectList;
			//for (auto& obj : world->getObjects())
			for (auto& obj : System::objectList[stepNumber])
			{
				float dist = glm::distance(activeCamera->getPos(), obj->getPos());
				orderedObjectList[dist] = obj;
			}

			//for (auto& emt : world->getEmitters())
			for (auto& emt : System::emitters[stepNumber])
			{
				for (auto& p : *emt->getParticleList())
				{
					orderedObjectList[p.first] = p.second;
				}
			}

			//enlazar objetos entrada/salida
			for (auto& bufferName : step.input)
			{
				auto readBackBuffer =
					((GL4Render*)render)->getBuffer(bufferName.second);
				for (auto& obj : orderedObjectList)
				{
					for (auto& mesh : obj.second->getMeshes())
					{
						mesh->getMaterial()->setTexture(bufferName.second, readBackBuffer);
					}
				}
			}
			//activar buffers de salida en render
			for (auto& bufferName : step.output)
			{
				((GL4Render*)render)->
					setOutBuffer(bufferName.first, bufferName.second);
			}

			//preparar buffer en render
			((GL4Render*)render)->setupFrameBuffer();

			//dibujar
			stepBegin = static_cast<float>(glfwGetTime());
			render->drawObjects(&orderedObjectList);
			stepEnd = static_cast<float>(glfwGetTime());

			if (stepNumber == System::stepNumber - 1)
				stepDeltaTime = stepEnd - stepBegin;

			stepNumber++;
		}

		glfwPollEvents();

		// CREATE WINDOW
		System::drawGUI();

		render->swapBuffer();

		if (System::getInputManager()->isPressed('E') || System::getInputManager()->isPressed('e'))
			System::exit();
	}
}

void System::initSystem()
{
	end = false;
	world = new World();
	modelMatrix = glm::mat4();
	render = FactoryEngine::getNewRender();
	inputManager = FactoryEngine::getNewInputManager();

	render->init();
	inputManager->init(((GLRender*)render)->getWindow());

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// TODO esto tendria que ir en otro sitio
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(((GLRender*)render)->getWindow(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();
}

void System::terminateSystem()
{
	glfwTerminate();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	//delete world;
	//delete render;
	//delete inputManager;
}

void System::addObject(Object* obj)
{
	world->addObject(obj);
}

Render* System::getRender()
{
	return render;
}

World* System::getWorld()
{
	return world;
}

void System::setWorld(World* world)
{
	System::world = world;
}

Object* System::getActiveObject()
{
	return activeObject;
}

void System::setActiveObject(Object* object3D)
{
	System::activeObject = object3D;
}

glm::mat4 System::getModelMatrix()
{
	return modelMatrix;
}

void System::setModelMatrix(glm::mat4 modelMatrix)
{
	System::modelMatrix = modelMatrix;
}

InputManager* System::getInputManager()
{
	return inputManager;
}

bool System::shouldExit()
{
	return end;
}

int System::getStepCount()
{
	return stepNumber;
}

void System::readPipeLine(std::string fileName)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fileName.c_str());

	if (!result)
	{
		return;
	}

	// Cargado correctamente, podemos analizar su contenido ...
	pugi::xml_node pipelineTree = doc.child("pipeline");
	for (pugi::xml_node stepNode = pipelineTree.child("step");
		stepNode;
		stepNode = stepNode.next_sibling("step"))
	{
		PipelineNode p;
		auto inputNode = stepNode.child("input");
		if (inputNode)
			for (auto input : inputNode.children())
				p.input[input.name()] = input.text().as_string();

		auto outputNode = stepNode.child("output");
		if (outputNode)
			for (auto output : outputNode.children())
				p.output[output.name()] = output.text().as_string();

		System::pipeline.push_back(p);
	}

	stepNumber = (int)System::pipeline.size();
}

void System::addLight(int pipelineStep, Light* l)
{
	if (!System::uniqueEntities.contains(l))
		System::world->addLight(l);

	System::lights[pipelineStep].push_back(l);
	System::uniqueEntities[l] = l;
}

void System::addCamera(int pipelineStep, Camera* c)
{
	if (!System::uniqueEntities.contains(c))
		System::world->addCamera(c);

	System::cameras[pipelineStep].push_back(c);
	System::uniqueEntities[c] = c;
}

void System::addEmitter(int pipelineStep, Emitter* e)
{
	if (!System::uniqueEntities.contains(e))
		System::world->addEmitter(e);

	System::emitters[pipelineStep].push_back(e);
	System::uniqueEntities[e] = e;
}

void System::addObject(int pipelineStep, Object3D* obj)
{
	if (!System::uniqueEntities.contains(obj))
		System::world->addObject(obj);

	System::objectList[pipelineStep].push_back(obj);
	System::render->setupObject(obj);
	System::uniqueEntities[obj] = obj;
}

void System::drawGUI()
{
	ImGui::Begin("Cloud configuation");

	std::string fps_s = "Fps: " + std::to_string(fps);
	ImGui::Text(fps_s.c_str());

	std::string dts = "Cloud render time: " + std::to_string(deltaTime);
	ImGui::Text(dts.c_str());

	//Light* sun = System::getWorld()->getLights()->at(0);
	//std::string rts = "rot ids : " + std::to_string((sun->getRot().y / (glm::pi<float>() * 2)) - .25f);
	//ImGui::Text(rts.c_str());


	if (ImGui::CollapsingHeader("General config"))
	{
		ImGui::SliderFloat("Brightness", &CloudConfiguration::brightness, 0.0f, 10.0f);
		ImGui::SliderFloat("Coverage attenuation", &CloudConfiguration::debugZ, 0.0f, 1.0f);
		ImGui::SliderInt("N samples", &CloudConfiguration::nSamples, 16, 256);
		ImGui::SliderFloat("World radius", &CloudConfiguration::worldRadius, 1000, 10000);
		ImGui::SliderFloat("Cloud sphere", &CloudConfiguration::cloudStartRange, 100, 3000);
		ImGui::SliderFloat("Cloud range", &CloudConfiguration::cloudRange, 500, 5000);
		ImGui::SliderFloat("Cloud speed", &CloudConfiguration::cloudMoveSpeed, 0, 50);
	}

	if (ImGui::CollapsingHeader("Light config"))
	{
		auto rot = world->getLights()->at(0)->getRot();
		float rotp[3] = { rot.x, rot.y, rot.z };
		ImGui::SliderFloat3("Sun roation", rotp, 0, glm::pi<float>() * 2);

		world->getLights()->at(0)->setRot({ rotp[0], rotp[1], rotp[2] });

		ImGui::SliderFloat("Light sample scale", &CloudConfiguration::lightSampleScale, 1, 20);
		ImGui::SliderInt("Light sample count", &CloudConfiguration::lightSampleCount, 1, 64);
	}

	if (ImGui::CollapsingHeader("Shape config"))
	{
		ImGui::SliderFloat("Cloud type", &CloudConfiguration::cloudType, 0, 2);
		ImGui::SliderFloat("Base UV scale", &CloudConfiguration::uvScaleBase, 1.0f, 20.f);
		ImGui::SliderFloat("Detail UV scale", &CloudConfiguration::uvScaleDetail, 1.0f, 100.0f);
		ImGui::SliderFloat("Coverage UV scale", &CloudConfiguration::uvScaleCoverage, 1.0f, 15.0f);
		ImGui::SliderFloat("Anvil bias", &CloudConfiguration::anvil_bias, 0.0f, 1.0f);
		ImGui::SliderFloat("Curl noise scale", &CloudConfiguration::curlNoiseScale, 0.0f, 10000.f * 2);
	}

	if (ImGui::CollapsingHeader("Phase config"))
	{
		ImGui::SliderFloat("Silver intensity", &CloudConfiguration::silver_intensity, 0.0f, 1.0f);
		ImGui::SliderFloat("Silver spread", &CloudConfiguration::silver_spread, 0.0f, 1.0f);
		ImGui::SliderFloat("Eccentrity", &CloudConfiguration::eccentrity, 0.0f, 1.0f);
	}

	if (ImGui::CollapsingHeader("Fog config"))
	{
		ImGui::SliderFloat("Fog density", &CloudConfiguration::fogDensity, 0.f, 1.f);
		ImGui::SliderFloat("Sky far mult", &CloudConfiguration::skyFarMultiplier, 1.f, 100.f);
	}

	ImGui::End();

	// TODO esto a clase render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

float System::getTime()
{
	return System::time;
}
