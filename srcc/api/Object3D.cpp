#include "Object3D.h"
#include "System.h"
#include "Mesh3D.h"
#include "GLRender.h"
#include "GLTextureFB.h"

TrianguloRot::TrianguloRot()
{
	std::vector<vertex_t>* vertexList_ = new std::vector<vertex_t>({
		vertex_t({ 0, 0.5f, 0, 1.0f },		{ 1.0f, 0.0f, 0.0f, 1.f }, glm::vec4(0), {.5f, 1.f}),
		vertex_t({ -0.5f, -0.5f, 0, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.f }, glm::vec4(0), {0.f, 0.f}),
		vertex_t({ 0.5f, -0.5f, 0, 1.0f },	{ 0.0f, 0.0f, 1.0f, 1.f }, glm::vec4(0), {1.f, 0.f}),
		});

	std::vector<uint32_t>* idxList = new std::vector<uint32_t>({ 0, 2, 1 });
	//std::vector<uint32_t>* idxList = new std::vector<uint32_t>({ 0, 1, 2 });

	this->addMesh(new Mesh3D("TrianguloRot", vertexList_, idxList));

	this->getMesh(0)->getMaterial()->getTexture()->load("./data/front.png");
}

void TrianguloRot::step(float deltaTime)
{
	auto currentRot = this->getRot();
	currentRot.y += (float)M_PI_2 * deltaTime; // Multiplicamos por pi medios para que gire a 90 grados por segundo
	this->setRot(currentRot);

	if (System::getInputManager()->isPressed('E') || System::getInputManager()->isPressed('e'))
		System::exit();
}

void Object3D::loadTextures(pugi::xml_node textureNode, Material* mat)
{
	auto typeAttrib = textureNode.attribute("type");
	std::string texType = Texture::COLOR_2D;

	if (typeAttrib)
		texType = typeAttrib.as_string();


	for (pugi::xml_node layerNode = textureNode.child("layer");
		layerNode;
		layerNode = layerNode.next_sibling("layer"))
	{
		std::string name = layerNode.attribute("name").as_string();
		mat->setTexture(name, layerNode.text().as_string(), texType);
	}

	//std::string layer = textureNode.text().as_string();
	//mat->setTexture(layer);
}

Material* Object3D::getMeshMaterial(pugi::xml_node material_description)
{
	Material* mat = FactoryEngine::getNewMaterial();

	glm::vec4 color(1);
	auto color_d = material_description.child("color");
	if (color_d)
	{
		auto colorVec = splitString<float>(color_d.text().as_string(), ',');
		color = glm::vec4(colorVec[0], colorVec[1], colorVec[2], colorVec[3]);
	}
	mat->color = color;

	uint8_t matShininess = 0;
	auto shininess = material_description.child("shininess");
	if (shininess)
	{
		matShininess = (uint8_t)shininess.text().as_int();
	}
	mat->setShininess(matShininess);

	bool light = false;
	auto lightNode = material_description.child("light");
	if (lightNode)
	{
		light = lightNode.text().as_bool();
	}
	mat->setLighting(light);

	bool culling = false;
	auto cullingNode = material_description.child("culling");
	if (cullingNode)
	{
		culling = cullingNode.text().as_bool();
	}
	mat->setCulling(culling);

	bool depthWrite = false;
	auto depthNode = material_description.child("depthWrite");
	if (depthNode)
	{
		depthWrite = depthNode.text().as_bool();
	}
	mat->setDepthWrite(depthWrite);

	Material::blendMode_e blendMode = Material::blendMode_e::NONE;
	auto matNode = material_description.child("blendMode");
	if (matNode)
	{
		if (matNode.text().as_string() == "alpha")
			blendMode = Material::blendMode_e::ALPHA;
		else if (matNode.text().as_string() == "add")
			blendMode = Material::blendMode_e::ADD;
		else if (matNode.text().as_string() == "mul")
			blendMode = Material::blendMode_e::MUL;
	}
	mat->setBlendMode(blendMode);

	auto refraction = material_description.child("refraction");
	if (refraction)
	{
		mat->setRefractionEnabled(refraction.text().as_bool());
	}

	auto refractIndex = material_description.child("refractIndex");
	if (refractIndex)
	{
		mat->setRefractionIndex(refractIndex.text().as_float());
	}

	auto reflection = material_description.child("reflection");
	if (reflection)
	{
		mat->setReflectionEnabled(reflection.text().as_bool());
	}

	auto texture = material_description.child("texture");
	if (texture)
	{
		this->loadTextures(texture, mat);
	}
	else
		mat->getProgram()->setColorTexDisable();

	auto shadow = material_description.child("shadow");
	if (shadow)
	{
		mat->setShadowEnabled(shadow.text().as_bool());
	}

	//// leer programas 
	//auto programList = splitString<std::string>(material_description.child("shader").text().as_string(), ',');
	//mat->loadPrograms(programList);

	return mat;
}

std::vector<glm::vec4>* Object3D::getMeshVertexList(pugi::xml_node vertexListDescription, int vertexCompCount)
{
	std::vector<glm::vec4>* vertList = new std::vector<glm::vec4>();

	std::string vListStr = vertexListDescription.text().as_string();

	std::vector<float> vectorList = splitString<float>(vListStr, ',');

	for (size_t i = 0; i < vectorList.size(); i += vertexCompCount)
	{
		std::vector<float> a = { 1.f, 2.f, 3.f };

		vertList->push_back({
			vectorList[i],
			vertexCompCount > 1 ? vectorList[i + 1] : 1,
			vertexCompCount > 2 ? vectorList[i + 2] : 1,
			vertexCompCount > 3 ? vectorList[i + 3] : 1
			});
	}

	return vertList;
}

std::vector<glm::vec2>* Object3D::getMeshTexCoordList(pugi::xml_node vertexListDescription)
{
	std::vector<glm::vec2>* vertList = new std::vector<glm::vec2>();

	std::string vListStr = vertexListDescription.text().as_string();

	std::vector<float> vectorList = splitString<float>(vListStr, ',');

	for (size_t i = 0; i < vectorList.size(); i += 2)
	{
		vertList->push_back({
			vectorList[i],
			vectorList[i + 1]
			});
	}

	return vertList;
}

std::vector<glm::uint32>* Object3D::getMeshIndexList(pugi::xml_node indexListDescription)
{
	std::string vListStr = indexListDescription.text().as_string();

	std::vector<glm::uint32> vectorList = splitString<glm::uint32>(vListStr, ',');

	return new std::vector<glm::uint32>(vectorList);
}

void Object3D::loadDataFromFile(std::string fileName)
{
	std::cout << "Loading: " << fileName << std::endl;

	if (fileName.ends_with(".msh"))
		loadMsh(fileName);
	else if (fileName.ends_with(".obj"))
		loadObj(fileName, FactoryEngine::getNewMaterial(), fileName);
}

void Object3D::loadDataFromFile(std::string file, bool sharedMesh)
{
	std::cout << "Loading: " << file << std::endl;

	if (file.ends_with(".msh"))
		loadMsh(file, sharedMesh);
	else if (file.ends_with(".obj"))
		loadObj(file, FactoryEngine::getNewMaterial(), file, sharedMesh);
}

void Object3D::loadObj(std::string obj, Material* mat, std::string mshFileName)
{
	this->loadObj(obj, mat, mshFileName, true);
}

void Object3D::loadObj(std::string objFile, Material* mat, std::string mshFileName, bool sharedMesh)
{
	std::cout << "Loading obj " << objFile << std::endl;

	//std::string textureName = obj;
	//size_t pointIdx = obj.find('.', 1); // ignoramos el primer caracter por si es path relativo
	//textureName.replace(pointIdx + 1, 3, "png");
	//mat->setTexture(textureName);

	std::ifstream f(objFile, std::ios_base::in);
	std::vector<glm::vec4> vPos;
	std::vector<glm::vec2> vTC;
	std::vector<glm::vec4> vNorm;
	std::vector<glm::vec4> vTangent;
	std::vector<glm::vec4> vBoneIdx;
	std::vector<glm::vec4> vBoneWeight;

	std::string line;
	Mesh3D* m = nullptr;
	int vertexOffset = 0;

	while (std::getline(f, line, '\n')) {
		std::istringstream str(line);
		std::string key;
		str >> key;
		if (key[0] != '#' || key == "#vta" || key == "#vboneidx" || key == "#vbonew") {
			if (key == "o")
			{
				if (m)
				{
					if (vNorm.size() == 0)
						m->recomputeNormals();
					addMesh(m);

					MeshManager::addMSH(mshFileName, m);
				}

				m = new Mesh3D(mshFileName);
				m->setMaterial(mat);
				vertexOffset = (int)vPos.size();
			}
			else if (key == "v")
			{
				glm::vec4 v(1.0f);
				str >> v.x >> v.y >> v.z;
				vPos.push_back(v);
				m->addVertex(vertex_t{});
			}
			else if (key == "vn")
			{
				glm::vec4 v(0);
				str >> v.x >> v.y >> v.z;
				vNorm.push_back(v);
			}
			else if (key == "vt")
			{
				glm::vec2 v(0);
				str >> v.x >> v.y;
				vTC.push_back(v);
			}
			else if (key == "f")
			{
				std::string vert;
				vertex_t v[3];
				int vIndex[3] = { 0 };
				for (int i = 0; i < 3; i++)
				{
					str >> vert;
					auto indexes = splitString<int>(vert, '/');

					glm::vec4 norm = { 0, 0, 0, 0 };
					if (vNorm.size() > 0)
						norm = vNorm[indexes[2] - 1];

					glm::vec2 tc = { 0,0 };
					if (vTC.size() > 0)
						tc = vTC[indexes[1] - 1];

					v[i] = { vPos[indexes[0] - 1], mat->color, norm, tc };

					if (vTangent.size() > (indexes[0] - 1))
					{
						v[i].tangent = vTangent[indexes[0] - 1];
					}

					if (vBoneIdx.size() > (indexes[0] - 1))
					{
						v[i].boneIdx = vBoneIdx[indexes[0] - 1];
					}

					if (vBoneWeight.size() > (indexes[0] - 1))
					{
						v[i].boneWeights = vBoneWeight[indexes[0] - 1];
					}

					m->setVertex(v[i], indexes[0] - 1 - vertexOffset);
					m->addId(indexes[0] - 1 - vertexOffset);
				}
			}
			else if (key == "#vta")
			{
				glm::vec4 v(0);
				str >> v.x >> v.y >> v.z;
				vTangent.push_back(v);
			}
			else if (key == "#vboneidx")
			{
				glm::ivec4 v(0);
				str >> v.x >> v.y >> v.z >> v.w;
				vBoneIdx.push_back(v);
			}
			else if (key == "#vbonew")
			{
				glm::vec4 v(0);
				str >> v.x >> v.y >> v.z >> v.w;
				vBoneWeight.push_back(v);
			}
		}
	}
	if (m)
	{
		if (vNorm.size() == 0)
			m->recomputeNormals();
		addMesh(m);

		MeshManager::addMSH(mshFileName, m);
	}

	std::cout << "Loaded obj " << objFile << std::endl;
}

void Object3D::loadMsh(std::string fileName)
{
	this->loadMsh(fileName, true);
}

void Object3D::loadMsh(std::string fileName, bool sharedMesh)
{
	MSH_t* msh_t = MeshManager::getMSH(fileName);
	if (msh_t != nullptr)
	{
		if (sharedMesh)
		{
			//std::cout << "same mesh" << std::endl;
			this->addMesh(msh_t->mesh);
			msh_t->numberOfUses++;
			return;
		}
		else
		{
			//std::cout << "copy mesh" << std::endl;
			this->addMesh(new Mesh3D(*msh_t->mesh));
			msh_t->numberOfUses++;
			return;
		}
	}
	// abrir/comprobar fichero
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fileName.c_str());
	if (!result)
	{
		std::cout << __FILE__ << ":" << __LINE__ << " " << result.description() << std::endl;
		std::cout << "File is " << fileName << std::endl;
		return;
	}

	pugi::xml_node meshNode = doc.child("mesh");
	pugi::xml_node buffersNode = meshNode.child("buffers");

	for (pugi::xml_node bufferNode = buffersNode.child("buffer");
		bufferNode;
		bufferNode = bufferNode.next_sibling("buffer"))
	{
		// por cada buffer leer material y leer meshData
		Material* mat = nullptr;
		pugi::xml_node material = bufferNode.child("material");
		if (material)
		{
			mat = getMeshMaterial(material);
		}

		std::vector<glm::vec4>* vertCoordList = nullptr;
		pugi::xml_node vertexCoordsList = bufferNode.child("coords");
		if (vertexCoordsList)
		{
			int vertexCompCount = atoi(vertexCoordsList.attribute("vertexCompCount").value());

			vertCoordList = this->getMeshVertexList(vertexCoordsList, vertexCompCount);
		}

		std::vector<glm::uint32>* idxList = nullptr;
		pugi::xml_node indexList = bufferNode.child("indices");
		if (indexList)
		{
			idxList = this->getMeshIndexList(indexList);
		}

		std::vector<glm::vec2>* texCoords = nullptr;
		pugi::xml_node texCoordsList = bufferNode.child("texCoords");
		if (texCoordsList)
		{
			int texCoordCompCount = atoi(texCoordsList.attribute("texCoordCompCount").value());

			texCoords = this->getMeshTexCoordList(texCoordsList);
		}

		pugi::xml_node meshData = bufferNode.child("meshData");
		if (meshData)
		{
			//loadObj(meshData.text().as_string(), mat);
			loadObj(meshData.text().as_string(), mat, fileName);
		}
		else
		{
			std::vector<vertex_t>* vertList = new std::vector<vertex_t>();

			for (size_t i = 0; i < vertCoordList->size(); i++)
			{
				vertList->push_back({
					vertCoordList->at(i),
					{1, 1, 1, 1},
					glm::vec4(0),
					texCoords ? texCoords->at(i) : glm::vec2({0, 0})
					});
			}

			Mesh3D* msh = new Mesh3D(fileName, vertList, idxList);
			//if (msh)
			msh->recomputeNormals();
			if (mat)
				msh->setMaterial(mat);

			this->addMesh(msh);

			MeshManager::addMSH(fileName, msh);
		}

		pugi::xml_node bonesBuffer = meshNode.child("bones");
		if (bonesBuffer)
		{
			this->armature = new Armature();
			this->armature->loadArmature(bonesBuffer);
		}

		pugi::xml_node framesBuffer = meshNode.child("lastFrame");
		if (framesBuffer)
		{
			this->numFrames = framesBuffer.text().as_int();
		}
	}
}

Object3D::Object3D(std::string fileName)
{
	loadDataFromFile(fileName);
}

void Object3D::setAnimate(bool animate)
{
	this->animate = animate;
}

bool Object3D::shouldAnimate()
{
	bool value = this->animate && this->armature != nullptr && this->numFrames > 0;
	return this->animate && this->armature != nullptr && this->numFrames > 0;
}

int Object3D::getFps() const
{
	return this->fps;
}

void Object3D::setFps(int fps)
{
	std::cout << 1.f / fps << " secons per frame" << std::endl;
	this->fps = fps;
}

int Object3D::getCurrentFrame() const
{
	return this->currentFrame;
}

void Object3D::setCurrentFrame(int frame)
{
	this->currentFrame = frame;
}

void Object3D::calculateAnimMatrices()
{
	this->armature->updateBones(this->currentFrame);
}

std::vector<glm::mat4>* Object3D::getAnimMatrices()
{
	std::vector<glm::mat4>* animMatrices = new std::vector<glm::mat4>();
	animMatrices->resize(this->armature->idList.size());

	int i = 0;
	for (const auto& b : this->armature->idList)
	{
		(*animMatrices)[b->id] = b->boneMatrix;
		i++;
	}

	return animMatrices;
}

void Object3D::step(float deltaTime)
{
	static float timeAccum = 0;
	timeAccum += deltaTime;
	if (this->shouldAnimate())
	{

		const float spf = 1.f / this->fps;

		if (timeAccum > spf)
		{
			this->currentFrame = (this->currentFrame + 1) % this->numFrames;
			if (this->currentFrame < 0)
				this->currentFrame = this->numFrames - 1;
			timeAccum -= spf;
			this->calculateAnimMatrices();
		}
	}

	if (System::getInputManager()->isPressed('E') || System::getInputManager()->isPressed('e'))
		System::exit();
}

CubeTex::CubeTex()
{
	std::vector<vertex_t>* topVertList = new std::vector<vertex_t>({
		vertex_t({ -0.5f,	-0.5f,	-0.5f, 1.0f },	{ 1.0f, 0.0f, 0.0f, 1.f }, glm::vec4(0), {0.f, 0.f}),
		vertex_t({ 0.5f,	-0.5f,	-0.5f, 1.0f },	{ 0.0f, 1.0f, 0.0f, 1.f }, glm::vec4(0), {1.f, 0.f}),
		vertex_t({ -0.5f,	-0.5f,	0.5f, 1.0f },	{ 0.0f, 0.0f, 1.0f, 1.f }, glm::vec4(0), {0.f, 1.f}),
		vertex_t({ 0.5f,	-0.5f,	0.5f, 1.0f },	{ 0.0f, 0.0f, 1.0f, 1.f }, glm::vec4(0), {1.f, 1.f}),
		vertex_t({ -0.5f,	0.5f,	-0.5f, 1.0f },	{ 1.0f, 0.0f, 0.0f, 1.f }, glm::vec4(0), {0.f, 0.f}),
		vertex_t({ 0.5f,	0.5f,	-0.5f, 1.0f },	{ 0.0f, 1.0f, 0.0f, 1.f }, glm::vec4(0), {1.f, 0.f}),
		vertex_t({ -0.5f,	0.5f,	0.5f, 1.0f },	{ 0.0f, 0.0f, 1.0f, 1.f }, glm::vec4(0), {0.f, 1.f}),
		vertex_t({ 0.5f,	0.5f,	0.5f, 1.0f },	{ 0.0f, 0.0f, 1.0f, 1.f }, glm::vec4(0), {1.f, 1.f}),
		});

	std::vector<glm::uint32>* topBotFaces = new std::vector<glm::uint32>({
		0, 1, 3, 0, 3, 2,
		7, 5, 4, 7, 4, 6
		});

	std::vector<vertex_t>* sideVertList = new std::vector<vertex_t>({
		vertex_t({ -0.5f,	-0.5f,	-0.5f, 1.0f },	{ 1.0f, 0.0f, 0.0f, 1.f }, glm::vec4(0), {1.f, 0.f}),
		vertex_t({ 0.5f,	-0.5f,	-0.5f, 1.0f },	{ 0.0f, 1.0f, 0.0f, 1.f }, glm::vec4(0), {0.f, 0.f}),
		vertex_t({ -0.5f,	-0.5f,	0.5f, 1.0f },	{ 0.0f, 0.0f, 1.0f, 1.f }, glm::vec4(0), {0.f, 0.f}),
		vertex_t({ 0.5f,	-0.5f,	0.5f, 1.0f },	{ 0.0f, 0.0f, 1.0f, 1.f }, glm::vec4(0), {1.f, 0.f}),
		vertex_t({ -0.5f,	0.5f,	-0.5f, 1.0f },	{ 1.0f, 0.0f, 0.0f, 1.f }, glm::vec4(0), {1.f, 1.f}),
		vertex_t({ 0.5f,	0.5f,	-0.5f, 1.0f },	{ 0.0f, 1.0f, 0.0f, 1.f }, glm::vec4(0), {0.f, 1.f}),
		vertex_t({ -0.5f,	0.5f,	0.5f, 1.0f },	{ 0.0f, 0.0f, 1.0f, 1.f }, glm::vec4(0), {0.f, 1.f}),
		vertex_t({ 0.5f,	0.5f,	0.5f, 1.0f },	{ 0.0f, 0.0f, 1.0f, 1.f }, glm::vec4(0), {1.f, 1.f}),
		});

	std::vector<glm::uint32>* sideFaces = new std::vector<glm::uint32>({
		0, 6, 4, 0, 2, 6,
		0, 5, 1, 0, 4, 5,
		1, 5, 7, 1, 7, 3,
		2, 7, 6, 2, 3, 7
		});

	this->addMesh(new Mesh3D("cubeTex", topVertList, topBotFaces));
	this->addMesh(new Mesh3D("cubeTex", sideVertList, sideFaces));

	this->getMesh(0)->recomputeNormals();
	this->getMesh(1)->recomputeNormals();

	this->getMesh(0)->getMaterial()->setTexture("./data/top.png");
	this->getMesh(0)->getMaterial()->getTexture()->update();
	this->getMesh(1)->getMaterial()->setTexture("./data/front.png");
	this->getMesh(1)->getMaterial()->getTexture()->update();


	//this->getMesh(1)->getMaterial()->getProgram()->setColorTexDisable();
}

CubeTex::CubeTex(std::string fileName)
{
	loadDataFromFile(fileName);
}

void CubeTex::step(float deltaTime)
{
	auto currentRot = this->getRot();
	currentRot.y += (float)M_PI_2 * deltaTime; // Multiplicamos por pi medios para que gire a 90 grados por segundo
	this->setRot(currentRot);

	if (System::getInputManager()->isPressed('Q'))
	{
		this->getMesh(0)->getMaterial()->getProgram()->setColorTexEnable();
		this->getMesh(1)->getMaterial()->getProgram()->setColorTexEnable();
	}

	if (System::getInputManager()->isPressed('E') || System::getInputManager()->isPressed('e'))
		System::exit();
}

MSH_t* MeshManager::getMSH(std::string name)
{
	if (!mshList.contains(name))
		return nullptr;

	return mshList[name];
}

void MeshManager::addMSH(std::string name, Mesh3D* msh)
{
	mshList[name] = new MSH_t{
		.numberOfUses = 1,
		.mesh = msh,
		.vertexCount = msh->getVertList()->size(),
		.idxCount = msh->getTriangleIdxList()->size()
	};
}

void FollowCamObject::step(float deltaTime)
{
	Object3D::step(deltaTime);

	Camera* cam = System::getWorld()->getCamera(System::getWorld()->getActiveCamera());

	this->setPos(cam->getPos());
}

Mirror::Mirror(int renderStep, Camera* mirrorCamera, Camera* playerCamera) : Object3D("./data/plane2D.obj")
{
	this->mirrorCamera = mirrorCamera;
	this->playerCamera = playerCamera;

	Material* mat = this->getMesh(0)->getMaterial();
	//GLTextureFB* tex = new GLTextureFB(
	//	Texture::textureType_e::colorBuffer, System::getRender()->getWidth(), System::getRender()->getHeight()
	//);

	GLTextureFB* tex = (GLTextureFB*)((GLRender*)System::getRender())->getBuffer(renderStep, Texture::colorBuffer);
	mat->setTexture("textureColor", tex);
	mat->setDepthWrite(true);
	mat->setCulling(true);
	mat->setIsMirror(true);
}

void Mirror::step(float deltaTime)
{
	glm::vec3 normal;
	glm::vec3 rot = this->getRot();

	normal.x = glm::cos(rot.x) * glm::cos(rot.y);
	normal.y = glm::sin(rot.x) * glm::cos(rot.y);
	normal.z = glm::sin(rot.y);

	glm::vec3 dir = glm::vec3(0);

	dir.x = glm::cos(rot.x) * glm::sin(rot.y);
	dir.y = glm::sin(rot.x);
	dir.z = -glm::cos(rot.x) * glm::cos(rot.y);

	dir = glm::normalize(dir);

	glm::vec3 v = this->getPos() - this->playerCamera->getPos();
	v = glm::reflect(v, dir);

	v = glm::normalize(v);

	this->mirrorCamera->setPos(this->getPos() - v);
	this->mirrorCamera->lookAt = this->getPos();
}

PostProcessPlane::PostProcessPlane(int renderStep, Camera* renderCamera) : Object3D("./data/plane2D.msh")
{
	this->renderCamera = renderCamera;

	Material* oldMat = this->getMesh(0)->getMaterial();
	Texture* t = oldMat->getTextures()["textureDepth"];

	this->getMesh(0)->setMaterial(new GLSLPostProcessMaterial());

	Material* mat = this->getMesh(0)->getMaterial();

	GLTextureFB* colorTex = (GLTextureFB*)((GLRender*)System::getRender())->getBuffer(renderStep, Texture::colorBuffer);
	mat->setTexture("textureColor", colorTex);

	//GLTextureFB* depthTex = (GLTextureFB*)((GLRender*)System::getRender())->getBuffer(renderStep, Texture::depthBuffer);
	mat->setTexture("textureDepth", t);

	GLTexture* noise = new GLTexture("./data/3d_noise/noise_.", ".png", 128);
	mat->setTexture("cloudNoise", noise);
}

void PostProcessPlane::step(float deltaTime)
{
}

BoxCloud::BoxCloud() : Object3D("./data/normalMapCubeObj.msh")
{
	this->getMesh(0)->setMaterial(new GLSLBoxCloudMaterial());
}

void BoxCloud::step(float deltaTime)
{
}
