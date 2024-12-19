#include "Particle.h"
#include "FactoryEngine.h"

Particle::Particle(std::string mshFile, const glm::vec3& vel, float spinVel, double lifetime, bool autofade)
{
	this->vel = vel;
	this->setSpin(spinVel);
	this->lifeTime = this->maxLifeTime = lifetime;
	this->autoFade = autofade;

	this->loadDataFromFile(mshFile);
}

Particle::Particle(std::string mshFile, const glm::vec3& vel, const glm::vec3& pos, float spinVel, float scale, double lifetime, bool autofade)
{
	this->vel = vel;
	this->setPos(pos);
	this->setSpin(spinVel);
	this->lifeTime = this->maxLifeTime = lifetime;
	this->autoFade = autofade;

	// si autofade, no se comparte material
	this->loadDataFromFile(mshFile, !autoFade);

	Texture* tex = this->getMesh(0)->getMaterial()->getTexture();

	if (tex == nullptr)
	{
		this->setSize({ scale, scale, scale });
		return;
	}

	glm::ivec2 texSize = tex->getSize();

	this->setSize({ scale * texSize.x, scale * texSize.y, scale });
}

float Particle::getRemainingLifetime() const
{
	return (float)this->lifeTime;
}

void Particle::step(float deltaTime)
{
	glm::vec3 rot = this->getRot();
	this->setRot(glm::vec3(
		rot.x,
		rot.y,
		rot.z += this->getSpin() * deltaTime
	));
	this->setPos(this->getPos() + this->vel * deltaTime); // pos += direction * speed * timeStep;
	this->lifeTime -= deltaTime;

	// NOTE hay problemas en caseo mesh es compartido enre particulas
	if (this->autoFade)
	{
		for (auto& msh : this->getMeshes())
		{
			auto mt = msh->getMaterial();
			msh->getMaterial()->color.a = (float)(this->lifeTime / this->maxLifeTime);
			if (msh->getMaterial()->color.a < 0)
				msh->getMaterial()->color.a = 0;
		}
	}
}