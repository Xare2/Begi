#include "VKMaterial.h"
#include "System.h"

VKMaterial::VKMaterial()
{
	this->program = new VKProgram();
	// this->texture = FactoryEngine::getNewTexture();
}

void VKMaterial::loadPrograms(std::vector<std::string> files)
{
	for (auto &f : files)
	{
		program->addProgram(f);
	}
	program->linkProgram();
}

void VKMaterial::prepare()
{
	VKProgram *vprog = (VKProgram *)program;

	vprog->setMatrixM(System::getModelMatrix());

	vprog->setMatrixV(System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getView());

	auto p = System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getProjection();
	p[1][1] *= -1;

	vprog->setMatrixP(p);

	vprog->setMat(this->shininess, this->color, 1);

	vprog->setScene(
		(int)System::getWorld()->getLights()->size(),
		System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getPos(),
		System::getWorld()->getAmbientColor());

	if (System::getWorld()->getLights()->size() > 0)
	{
		int i = 0;
		for (i = 0; i < System::getWorld()->getLights()->size(); i++)
		{
			Light *l = System::getWorld()->getLights()->at(i);
			if (l->getEnabled())
				vprog->setLight(l, i);
			else
				vprog->turnOffLight(i);
		}
	}

	vprog->setColorTex();

	int countText = 0;
	for (auto &t : this->textures)
	{
		std::string name = t.first;
		auto texture = t.second;
		switch (texture->getType())
		{
		case Texture::textureType_e::colorBuffer:
			vprog->fbo.mat.usetextureColor = 1;
			program->bindTextureSampler(1, t.second);
		case Texture::textureType_e::depthBuffer:
			vprog->fbo.mat.usetextureDepth = 1;
			program->bindTextureSampler(1, t.second);
		case Texture::textureType_e::color2D:
			vprog->fbo.mat.usetextureColor = 1;
			program->bindTextureSampler(1, t.second);
			break;
		case Texture::textureType_e::cubic:
			vprog->fbo.mat.usecubetextureColor = 1;
			program->bindTextureSampler(1, t.second);
			break;
		default:
			std::cout << "Asigna tipo!!!" << std::endl;
		};
		countText++;
	}

	if (this->shadowEnabled)
	{
		glm::mat4 depthBiasMat(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f);
		glm::mat4 depthViewMatrix = System::getWorld()->getCamera(0)->getView();
		glm::mat4 depthProjMatrix = System::getWorld()->getCamera(0)->getProjection();
		depthBiasMat = depthBiasMat * depthProjMatrix *
					   depthViewMatrix * System::getModelMatrix();
		vprog->ubo.depthBias = depthBiasMat;
	}

	vprog->commitVertexBufferObject();
	vprog->commitFragmentBufferObject();
}

void VKMaterial::setTexture(std::string filename)
{
	textures[filename] = new VKTexture(filename);
}

void VKMaterial::setTexture(std::string name, std::string filename, std::string type)
{
	std::cout << "texture " << name << std::endl;
	if (type == Texture::COLOR_2D)
	{
		textures[name] = new VKTexture(filename);
	}
	else if (type == Texture::CUBE_MAP)
	{
		auto files = splitString<std::string>(filename, ',');
		textures[name] = new VKTexture(files[0], files[1], files[2], files[3], files[4], files[5]);
	}
	else
	{
		std::cout << "Texture type not implemented" << std::endl;
	}
}
