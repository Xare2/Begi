#include "Material.h"
#include "System.h"
#include "CloudConfiguration.h"

GLSLMaterial::GLSLMaterial()
{
	program = new GLProgram();
	//this->texture = FactoryEngine::getNewTexture();
	//this->texture = nullptr;
	loadPrograms({
		"./data/shader.vert",
		"./data/shader.frag",
		});
	this->color = { 1,1,1,1 };
}

GLSLMaterial::GLSLMaterial(std::string vert, std::string frag)
{
	program = new GLProgram();
	//this->texture = FactoryEngine::getNewTexture();
	//this->texture = nullptr;
	loadPrograms({
		vert,
		frag,
		});
	this->color = { 1,1,1,1 };
}

void GLSLMaterial::copyValues(Material* from)
{
	this->blendMode = from->getBlendMode();
	this->color = from->color;
	this->color.a = 1;
	this->culling = from->getCulling();
	this->depthWrite = from->getDepthWrite();
	this->lighting = from->getLighting();
	this->shininess = from->getShininess();
	this->textures = std::map<std::string, Texture*>(from->getTextures());
}

void GLSLMaterial::loadPrograms(std::vector<std::string> files)
{
	for (auto& f : files) {
		program->addProgram(f);
	}
	program->linkProgram();
}

void GLSLMaterial::prepare()
{
	program->use();

	glm::mat4 MVP = System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getProjection() *
		System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getView() * System::getModelMatrix();

	program->setMatrix("MVP", MVP);
	program->setMatrix("M", System::getModelMatrix());

	program->setInt("mat.enable", 1);
	program->setVec4("mat.color", this->color);
	program->setInt("mat.shinny", this->shininess);
	program->setInt("mat.receiveLight", this->lighting);
	program->setInt("mat.reflectionEnable", this->reflectionEnabled);
	program->setInt("mat.refractionEnable", this->refractionEnabled);
	program->setFloat("mat.refractionIndex", this->refractIndex);

	program->setInt("nLights", (int)System::getWorld()->getLights()->size());
	program->setVec3("cameraPos", System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getPos());
	program->setVec3("ambientColor", System::getWorld()->getAmbientColor());

	program->setInt("useLight", this->lighting);

	switch (this->blendMode)
	{
		//mezcla aditiva:
	case blendMode_e::ADD:
		glBlendFunc(GL_ONE, GL_ONE);
		break;
		//mezcla alpha
	case blendMode_e::ALPHA:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
		//mezcla multiplicativa:
	case blendMode_e::MUL:
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		break;
	}

	if (this->depthWrite)
	{
		glDepthMask(GL_TRUE);
	}
	else
		glDepthMask(GL_FALSE);

	if (this->culling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	((GLProgram*)program)->setVertexPos(sizeof(vertex_t), (void*)offsetof(vertex_t, pos), 4, GL_FLOAT);
	((GLProgram*)program)->setVertexColor(sizeof(vertex_t), (void*)offsetof(vertex_t, color), 4, GL_FLOAT);
	((GLProgram*)program)->setVertexNormal(sizeof(vertex_t), (void*)offsetof(vertex_t, normal), 4, GL_FLOAT);
	((GLProgram*)program)->setVertexCoordText(sizeof(vertex_t), (void*)offsetof(vertex_t, texCoords), 2, GL_FLOAT);
	((GLProgram*)program)->setVertexTangent(sizeof(vertex_t), (void*)offsetof(vertex_t, tangent), 4, GL_FLOAT);
	((GLProgram*)program)->setVertexBonesIdx(sizeof(vertex_t), (void*)offsetof(vertex_t, boneIdx), 4, GL_FLOAT);
	((GLProgram*)program)->setVertexBonesWeights(sizeof(vertex_t), (void*)offsetof(vertex_t, boneWeights), 4, GL_FLOAT);

	if (System::getWorld()->getLights()->size() > 0 && this->lighting)
	{
		for (int i = 0; i < (int)System::getWorld()->getLights()->size(); i++)
		{
			Light* l = System::getWorld()->getLights()->at(i);
			if (l->getEnabled() && this->lighting)
				program->setLight(l, i);
			else
				((GLProgram*)program)->setInt("lights[0].enable", 0, i, Light::LIGHT_STRIDE);
		}
	}

	// resetar variables texturea
	program->setInt("textureColor", 0);
	program->setInt("textureNormal", 0);
	program->setInt("textureDepth", 0);
	program->setInt("cubetextureColor", 3);

	program->setInt("mat.usetextureColor", 0);
	program->setInt("mat.usecubetextureColor", 0);
	program->setInt("mat.usetextureNormal", 0);
	program->setInt("mat.usetextureDepth", 0);
	program->setInt("mat.isMirror", this->isMirror ? 1 : 0);

	int countText = 0;
	for (auto& t : textures)
	{
		std::string name = t.first;
		auto texture = t.second;
		switch (texture->getType())
		{
		case GLTexture::textureType_e::colorBuffer:
		case GLTexture::textureType_e::depthBuffer:
		case GLTexture::textureType_e::color2D:
			//texture->bind(countText);
			((GLProgram*)program)->bindTextureSampler(countText, t.second);
			//t.second->cubic
			program->setInt(name, countText);
			program->setInt("mat.use" + name, 1);
			break;
		case GLTexture::textureType_e::cubic:
			//texture->bind(countText + 3);//3 primeras texturas de color,
			((GLProgram*)program)->bindTextureSampler(countText + 3, t.second);
			program->setInt("cube" + name, countText + 3);
			program->setInt("mat.usecube" + name, 1);
			break;
		default:
			std::cout << "Asigna tipo!!!" << std::endl;
		};
		countText++;
	}

	Object3D* obj = (Object3D*)System::getActiveObject();

	if (obj->shouldAnimate())
	{
		program->setInt("skinned", 1);
		auto animMatrices = obj->getAnimMatrices();

		for (size_t i = 0; i < animMatrices->size(); i++)
		{
			std::string name = "animMatrices[" + std::to_string(i) + "]";
			program->setMatrix(name, animMatrices->at(i));
		}
	}
	else
	{
		program->setInt("skinned", 0);
	}

	if (this->shadowEnabled)
	{
		glm::mat4 depthBiasMat(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
		);
		glm::mat4 depthViewMatrix = System::getWorld()->getCamera(0)->getView();
		glm::mat4 depthProjMatrix = System::getWorld()->getCamera(0)->getProjection();
		depthBiasMat = depthBiasMat * depthProjMatrix *
			depthViewMatrix * System::getModelMatrix();
		program->setMatrix("depthBias", depthBiasMat);
		program->setInt("mat.shadowEnable", 1);
	}
	//if (textures.size() > 0)
	//{
	//	for (auto& t : this->textures)
	//	{
	//		if (t.first == Texture::COLOR_2D)
	//		{
	//			program->bindTextureSampler(0, t.second);
	//			((GLProgram*)program)->setColorTexSampler(0, 1);
	//		}
	//		else if (t.first == Texture::CUBE_MAP)
	//		{
	//			program->bindTextureSampler(1, t.second);
	//			((GLProgram*)program)->setColorCubicSampler(1, 1);
	//		}
	//		else if (t.first == Texture::NORMAL_MAP)
	//		{
	//			program->bindTextureSampler(2, t.second);
	//			program->setInt("textureNormal", 2);
	//			program->setInt("mat.useNormalTex", 1);
	//		}
	//	}
	//}
	//else
	//{
	//	((GLProgram*)program)->setColorTexSampler(0, 0);
	//	((GLProgram*)program)->setColorCubicSampler(1, 0);
	//	program->setInt("mat.useNormalTex", 0);
	//}
}

VKMaterial::VKMaterial()
{
	this->program = new VKProgram();
	//this->texture = FactoryEngine::getNewTexture();
}

void VKMaterial::loadPrograms(std::vector<std::string> files)
{
	for (auto& f : files) {
		program->addProgram(f);
	}
	program->linkProgram();
}

void VKMaterial::prepare()
{
	VKProgram* vprog = (VKProgram*)program;

	vprog->setMatrixM(System::getModelMatrix());

	vprog->setMatrixV(System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getView());

	auto p = System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getProjection();
	p[1][1] *= -1;

	vprog->setMatrixP(p);

	vprog->setMat(this->shininess, this->color, 1);

	vprog->setScene(
		(int)System::getWorld()->getLights()->size(),
		System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getPos(),
		System::getWorld()->getAmbientColor()
	);

	if (System::getWorld()->getLights()->size() > 0)
	{
		int i = 0;
		for (i = 0; i < System::getWorld()->getLights()->size(); i++)
		{
			Light* l = System::getWorld()->getLights()->at(i);
			if (l->getEnabled())
				vprog->setLight(l, i);
			else
				vprog->turnOffLight(i);
		}

		//for (i = i; i < Light::N_LIGHTS; i++)
		//	vprog->turnOffLight(i);
	}

	vprog->setColorTex();

	vprog->commitVertexBufferObject();
	vprog->commitFragmentBufferObject();
}

void VKMaterial::setTexture(std::string filename)
{
	textures[filename] = new VKTexture(filename);
}

Texture* Material::getTexture()
{
	return textures.begin()->second;
}

Texture* Material::getTexture(std::string name)
{
	return this->textures[name];
}

std::map<std::string, Texture*> Material::getTextures()
{
	return this->textures;
}

void Material::setTexture(std::string name, Texture* tex)
{
	this->textures[name] = tex;
}

uint8_t Material::getShininess() const
{
	return this->shininess;
}

void Material::setShininess(uint8_t shininess)
{
	this->shininess = shininess;
}

Material::blendMode_e Material::getBlendMode() const
{
	return this->blendMode;
}

void Material::setBlendMode(blendMode_e blendMode)
{
	this->blendMode = blendMode;
}

bool Material::getLighting() const
{
	return this->lighting;
}

void Material::setLighting(bool enable)
{
	this->lighting = enable;
}

bool Material::getCulling() const
{
	return this->culling;
}

void Material::setCulling(bool enable)
{
	this->culling = enable;
}

bool Material::getDepthWrite() const
{
	return this->depthWrite;
}

void Material::setDepthWrite(bool enable)
{
	this->depthWrite = enable;
}

bool Material::getReflectionEnabled() const
{
	return this->reflectionEnabled;
}

void Material::setReflectionEnabled(bool enabled)
{
	this->reflectionEnabled = enabled;
}

bool Material::getRefractionEnabled() const
{
	return this->refractionEnabled;
}

void Material::setRefractionEnabled(bool enabled)
{
	this->refractionEnabled = enabled;
}

float Material::getRefractionIndex() const
{
	return this->refractIndex;
}

void Material::setRefractionIndex(float value)
{
	this->refractIndex = value;
}

bool Material::getShadowEnabled() const
{
	return this->shadowEnabled;
}

void Material::setShadowEnabled(bool enabled)
{
	this->shadowEnabled = enabled;
}

void Material::setIsMirror(bool value)
{
	this->isMirror = value;
}

void GLSLMaterial::setTexture(std::string filename)
{
	//texture = new GLTexture(filename);
	textures[filename] = new GLTexture(filename);
}

void GLSLMaterial::setTexture(std::string name, std::string filename, std::string type)
{
	std::cout << "texture " << name << std::endl;
	if (type == Texture::COLOR_2D)
	{
		textures[name] = new GLTexture(filename);
	}
	else if (type == Texture::CUBE_MAP)
	{
		auto files = splitString<std::string>(filename, ',');
		textures[name] = new GLTexture(files[0], files[1], files[2], files[3], files[4], files[5]);
	}
	//if (type == "color2D")
	//	textures[name] = new GLTexture(filename);
	//if (type == "cubeMap")
	//{
	//	auto files = splitString<std::string>(filename, ',');
	//	textures[name] = new GLTexture(files[0], files[1], files[2], files[3], files[4], files[5]);
	//}
}

GLTexture* GLSLMaterial::getReflectionTexture() const
{
	return (GLTexture*)textures.at(Texture::REFLECTION);
}

void GLSLMaterial::setReflectionTexture(GLTexture* tex)
{
	this->textures[Texture::REFLECTION] = tex;
}

GLTexture* GLSLMaterial::getRefractionTexture() const
{
	return (GLTexture*)textures.at(Texture::REFRACTION);
}

void GLSLMaterial::setRefractionTexture(GLTexture* tex)
{
	this->textures[Texture::REFRACTION] = tex;
}

GLTexture* GLSLMaterial::getNormalTexture() const
{
	return (GLTexture*)textures.at(Texture::NORMAL_MAP);
}

void GLSLMaterial::setNormalTexture(GLTexture* tex)
{
	this->textures[Texture::NORMAL_MAP] = tex;
}

float GLSLMaterial::getRefractionCoef() const
{
	return this->getRefractionIndex();
}

void GLSLMaterial::setRefractionCoef(float coef)
{
	this->setRefractionIndex(coef);
}

void GLSLPostProcessMaterial::prepare()
{
	// TODO le hara falta camera pos
	program->use();

	Camera* activeCamera = System::getWorld()->getCamera();

	activeCamera->computeModelMatrix();
	glm::vec3 camEulerAngles = activeCamera->getRot();

	camEulerAngles.x *= -1;

	// Convert to radians
	//camEulerAngles.x = camEulerAngles.x * pi / 180;
	//camEulerAngles.y = camEulerAngles.y * pi / 180;
	//camEulerAngles.z = camEulerAngles.z * pi / 180;
	//std::cout << System::getWorld()->getActiveCamera() << std::endl;
	//std::cout << "X: " << camEulerAngles.x << " Y: " << camEulerAngles.y << "\n";

	// Generate Quaternian
	glm::quat camRotation;
	camRotation = glm::quat(camEulerAngles);

	// Generate rotation matrix from quaternian
	glm::mat4 camToWorldMatrix = glm::toMat4(camRotation);

	((GLProgram*)program)->setVertexPos(sizeof(vertex_t), (void*)offsetof(vertex_t, pos), 4, GL_FLOAT);
	//((GLProgram*)program)->setVertexNormal(sizeof(vertex_t), (void*)offsetof(vertex_t, normal), 4, GL_FLOAT);
	((GLProgram*)program)->setVertexCoordText(sizeof(vertex_t), (void*)offsetof(vertex_t, texCoords), 2, GL_FLOAT);

	glm::mat4 MVP = System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getProjection() *
		System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getView() * System::getModelMatrix();

	//program->setMatrix("MVP", MVP);

	//program->setFloat("camRadius", Camera::NEAR);
	program->setVec3("camPos", System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getPos());
	//program->setVec3("boxPos", System::getActiveObject()->getPos());
	//program->setVec3("boxSize", System::getActiveObject()->getSize());
	//program->setMatrix("M", System::getModelMatrix());
	program->setMatrix("P", activeCamera->getProjection());
	program->setMatrix("V", activeCamera->getStaticView());
	program->setMatrix("camRotation", camToWorldMatrix);
	Light* sun = System::getWorld()->getLights()->at(0);

	glm::vec3 sunDir = sun->getDir();
	glm::vec3 sunPos = sun->getPos() - sunDir * 10000000.f;

	//program->setVec3("sunPos", sunPos);
	program->setVec3("sunDir", sunDir);
	program->setVec3("sunColor", glm::vec3(sun->getColor()));
	program->setFloat("timeOfDay", (sun->getRot().x / (glm::pi<float>() * 2)) - .25f);

	program->setFloat("time", System::getTime());
	program->setFloat("starting_noise", (int)(System::getTime() * 1000));
	program->setFloat("brightness", CloudConfiguration::brightness);
	program->setFloat("silver_intensity", CloudConfiguration::silver_intensity);
	program->setFloat("silver_spread", CloudConfiguration::silver_spread);
	program->setFloat("eccentrity", CloudConfiguration::eccentrity);
	program->setFloat("anvil_bias", CloudConfiguration::anvil_bias);
	program->setFloat("curlNoiseScale", CloudConfiguration::curlNoiseScale);
	program->setFloat("uvScaleBase", CloudConfiguration::uvScaleBase);
	program->setFloat("uvScaleDetail", CloudConfiguration::uvScaleDetail);
	program->setFloat("uvScaleCoverage", CloudConfiguration::uvScaleCoverage);
	program->setInt("nSamples", CloudConfiguration::nSamples);
	program->setFloat("lightSampleScale", CloudConfiguration::lightSampleScale);
	program->setInt("lightSampleCount", CloudConfiguration::lightSampleCount);
	program->setFloat("cloudType", CloudConfiguration::cloudType);

	program->setFloat("cloudMoveSpeed", CloudConfiguration::cloudMoveSpeed);
	program->setFloat("debugZ", CloudConfiguration::debugZ);
	program->setFloat("skyFarMultiplier", CloudConfiguration::skyFarMultiplier);
	program->setFloat("fogDensity", CloudConfiguration::fogDensity);
	program->setFloat("worldRadius", CloudConfiguration::worldRadius);
	program->setFloat("cloudStartRange", CloudConfiguration::cloudStartRange);
	program->setFloat("cloudEndRange", CloudConfiguration::cloudStartRange + CloudConfiguration::cloudRange);

	int countText = 0;
	for (auto& t : textures)
	{
		std::string name = t.first;
		Texture* texture = t.second;

		switch (texture->getType())
		{
		case GLTexture::textureType_e::colorBuffer:
		case GLTexture::textureType_e::depthBuffer:
		case GLTexture::textureType_e::color2D:
		case GLTexture::textureType_e::color3D:
			((GLProgram*)program)->bindTextureSampler(countText, t.second);
			program->setInt(name, countText);
			break;
		case GLTexture::textureType_e::cubic:
			((GLProgram*)program)->bindTextureSampler(countText + 3, t.second);
			program->setInt(name, countText + 3);
			break;
		default:
			std::cout << "Asigna tipo!!!" << std::endl;
		};
		countText++;
	}

	glEnable(GL_CULL_FACE);
}

GLSLPostProcessMaterial::GLSLPostProcessMaterial()
{
	program = new GLProgram();
	//this->texture = FactoryEngine::getNewTexture();
	//this->texture = nullptr;
	loadPrograms({
		"./data/sky_shader.vert",
		"./data/sky_shader.frag",
		});
	this->color = { 1,1,1,1 };

	this->textures["curlNoise"] = new GLTexture("./data/curl_noise.png");
	this->textures["weatherMap"] = new GLTexture("./data/coverage_F.png");
	this->textures["skyGradient"] = new GLTexture("./data/sky_gradient.png");
	this->textures["cloudNoise"] = new GLTexture("./data/3d_noise/noise_.", ".png", 128);
	this->textures["detailNoise"] = new GLTexture("./data/secondary_3d_noise/noise_", ".png", 32);
	////GLTexture(const std::string & left, const std::string & right,
	////	const std::string & front, const std::string & back,
	////	const std::string & top, const std::string & bottom);
	//this->textures["cubetextureColor"] = new GLTexture(
	//	"./datasky_left.png",
	//	"./datasky_right.png",
	//	"./datasky_front.png",
	//	"./datasky_back.png",
	//	"./datasky_top.png",
	//	"./datasky_bottom.png"
	//);
}

GLSLBoxCloudMaterial::GLSLBoxCloudMaterial()
{
	program = new GLProgram();
	//this->texture = FactoryEngine::getNewTexture();
	//this->texture = nullptr;
	loadPrograms({
		"./databox_shader.vert",
		"./databox_shader.frag",
		});
	this->color = { 1,1,1,1 };

	this->textures["curlNoise"] = new GLTexture("./datacurl_noise.png");
	this->textures["weatherMap"] = new GLTexture("./datacoverage_F.png");
	this->textures["cloudNoise"] = new GLTexture("./data3d_noise/noise_.", ".png", 128);
	this->textures["detailNoise"] = new GLTexture("./datasecondary_3d_noise/noise_", ".png", 32);
	
	this->textures["cubetextureColor"] = new GLTexture(
		"./datasky_left.png",
		"./datasky_right.png",
		"./datasky_front.png",
		"./datasky_back.png",
		"./datasky_top.png",
		"./datasky_bottom.png"
	);
}

void GLSLBoxCloudMaterial::prepare()
{
	program->use();

	Camera* activeCamera = System::getWorld()->getCamera();

	activeCamera->computeModelMatrix();
	glm::vec3 camEulerAngles = activeCamera->getRot();

	camEulerAngles.x *= -1;

	// Convert to radians
	//camEulerAngles.x = camEulerAngles.x * pi / 180;
	//camEulerAngles.y = camEulerAngles.y * pi / 180;
	//camEulerAngles.z = camEulerAngles.z * pi / 180;
	//std::cout << System::getWorld()->getActiveCamera() << std::endl;
	//std::cout << "X: " << camEulerAngles.x << " Y: " << camEulerAngles.y << "\n";

	// Generate Quaternian
	glm::quat camRotation;
	camRotation = glm::quat(camEulerAngles);

	// Generate rotation matrix from quaternian
	glm::mat4 camToWorldMatrix = glm::toMat4(camRotation);

	((GLProgram*)program)->setVertexPos(sizeof(vertex_t), (void*)offsetof(vertex_t, pos), 4, GL_FLOAT);
	((GLProgram*)program)->setVertexNormal(sizeof(vertex_t), (void*)offsetof(vertex_t, normal), 4, GL_FLOAT);
	((GLProgram*)program)->setVertexCoordText(sizeof(vertex_t), (void*)offsetof(vertex_t, texCoords), 2, GL_FLOAT);

	glm::mat4 MVP = System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getProjection() *
		System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getView() * System::getModelMatrix();

	program->setMatrix("MVP", MVP);

	//program->setFloat("camRadius", Camera::NEAR);
	program->setVec3("camPos", System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getPos());
	program->setVec3("boxPos", System::getActiveObject()->getPos());
	program->setVec3("boxSize", System::getActiveObject()->getSize());
	program->setMatrix("M", System::getModelMatrix());
	//program->setMatrix("P", activeCamera->getProjection());
	//program->setMatrix("V", activeCamera->getView());
	//program->setMatrix("camRotation", camToWorldMatrix);
	Light* sun = System::getWorld()->getLights()->at(0);

	glm::vec3 sunDir = sun->getDir();
	glm::vec3 sunPos = sun->getPos() - sunDir * 10000000.f;

	program->setVec3("sunPos", sunPos);
	program->setVec3("sunDir", sunDir);
	program->setVec3("sunColor", glm::vec3(sun->getColor()));
	program->setVec3("ambientColor", System::getWorld()->getAmbientColor());

	program->setFloat("time", System::getTime());
	program->setFloat("starting_noise", (int)(System::getTime() * 1000));
	program->setFloat("brightness", CloudConfiguration::brightness);
	program->setFloat("silver_intensity", CloudConfiguration::silver_intensity);
	program->setFloat("silver_spread", CloudConfiguration::silver_spread);
	program->setFloat("eccentrity", CloudConfiguration::eccentrity);
	//program->setFloat("anvil_bias", CloudConfiguration::anvil_bias);
	program->setFloat("curlNoiseScale", CloudConfiguration::curlNoiseScale);
	program->setFloat("uvScaleBase", CloudConfiguration::uvScaleBase);
	program->setFloat("uvScaleDetail", CloudConfiguration::uvScaleDetail);
	program->setInt("nSamples", CloudConfiguration::nSamples);
	program->setFloat("lightSampleScale", CloudConfiguration::lightSampleScale);
	program->setInt("lightSampleCount", CloudConfiguration::lightSampleCount);
	program->setInt("cloudType", CloudConfiguration::cloudType);


	int countText = 0;
	for (auto& t : textures)
	{
		std::string name = t.first;
		Texture* texture = t.second;

		switch (texture->getType())
		{
		case GLTexture::textureType_e::colorBuffer:
		case GLTexture::textureType_e::depthBuffer:
		case GLTexture::textureType_e::color2D:
		case GLTexture::textureType_e::color3D:
			((GLProgram*)program)->bindTextureSampler(countText, t.second);
			program->setInt(name, countText);
			break;
		case GLTexture::textureType_e::cubic:
			((GLProgram*)program)->bindTextureSampler(countText + 3, t.second);
			program->setInt(name, countText + 3);
			break;
		default:
			std::cout << "Asigna tipo!!!" << std::endl;
		};
		countText++;
	}

	glEnable(GL_CULL_FACE);
}
