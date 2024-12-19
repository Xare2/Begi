#include "Emitter.h"
#include "System.h"

Emitter::Emitter(const std::string mshFile, float emissionRate, bool autofade)
{
	this->fade = autofade;
	this->minRate = emissionRate;
	this->maxRate = emissionRate;

	this->emit = true;

	this->mshFile = mshFile;

	this->minSpin = 0;
	this->maxSpin = 0;

	this->minScale = 0;
	this->maxScale = 0;

	this->minLifetime = 0;
	this->maxLifetime = 0;

	this->minVel = glm::vec3({ 0.f, 0.f, 0.f });
	this->maxVel = glm::vec3({ 0.f, 0.f, 0.f });

	this->minColor = glm::vec4({ 0.f, 0.f, 0.f, 0.f });
	this->maxColor = glm::vec4({ 0.f, 0.f, 0.f, 0.f });

	this->particleList = new std::map<float, Particle*>();
}

void Emitter::setRateRange(float min, float max)
{
	this->minRate = min;
	this->maxRate = max;
}

void Emitter::setVelocityRange(const glm::vec3& min, const glm::vec3& max)
{
	this->minVel = min;
	this->maxVel = max;
}

void Emitter::setSpinVelocityRange(float min, float max)
{
	this->minSpin = min;
	this->maxSpin = max;
}

void Emitter::setScaleRange(float min, float max)
{
	this->minScale = min;
	this->maxScale = max;
}

void Emitter::setLifetimeRange(float min, float max)
{
	this->minLifetime = min;
	this->maxLifetime = max;
}

void Emitter::setColorRange(const glm::vec4& min, const glm::vec4& max)
{
	this->minColor = min;
	this->maxColor = max;
}

void Emitter::setEmit(bool enable)
{
	this->emit = enable;
}

bool Emitter::isEmitting()
{
	return this->emit;
}

std::map<float, Particle*>* Emitter::getParticleList()
{
	return this->particleList;
}

void Emitter::step(float deltaTime)
{
	int partToEmit = 0;
	if (this->emit)
	{
		float emitterRate = glm::linearRand(this->minRate, this->maxRate);

		numParticulasNuevas += deltaTime * emitterRate;
		partToEmit = (int)numParticulasNuevas;
		numParticulasNuevas -= (int)partToEmit;
	}

	Camera* activeCamera = System::getWorld()->getCamera(System::getWorld()->getActiveCamera());

	for (int i = 0; i < partToEmit; i++)
	{
		Particle* p = new Particle(
			mshFile,
			glm::linearRand(this->minVel, this->maxVel),
			this->getPos(),
			glm::linearRand(this->minSpin, this->maxSpin),
			glm::linearRand(this->minScale, this->maxScale),
			glm::linearRand(this->minLifetime, this->maxLifetime),
			this->fade
		);
		// setup en render
		//System::render->setupObject(*p);
		System::getRender()->setupObject(p);

		p->step(0.05f);
		float dist = glm::distance(activeCamera->getPos(), p->getPos());
		//(*this->particleList)[dist] = p;
		this->particleList->insert(std::pair<float, Particle*>(dist, p));
	}

	std::map<float, Particle*> aux;
	//actualizar lista
	for (auto& p : *particleList)
	{
		if (p.second->getRemainingLifetime() > 0)
		{
			//step en particulas
			p.second->step(deltaTime);
			float dist = glm::distance(activeCamera->getPos(), p.second->getPos());
			aux[dist] = p.second;
		}
		else
		{
			System::getRender()->removeObject(p.second);
			delete p.second;
			//if (p.second->shouldDestroy())
			//{
			//}
		}
	}

	//ordenar por distancia a camara
	particleList->clear();
	particleList->insert(aux.begin(), aux.end());
}

Object* Emitter::getAllParticlesObject() const
{
	Object3D* toReturn = new Object3D();

	for (auto& p : *this->particleList)
	{
		Particle* part = p.second;
		for (size_t i = 0; i < part->getMeshes().size(); i++)
		{
			if (i >= toReturn->getMeshes().size())
			{
				toReturn->addMesh(new Mesh3D(this->mshFile));
			}
			Mesh3D* m = toReturn->getMeshes()[i];

			for (auto& v : *part->getMeshes()[i]->getVertList())
			{
				m->addVertex(v);
			}

			std::vector<glm::uint32>* triangleIdxList = part->getMeshes()[i]->getTriangleIdxList();
			for (size_t t = 0; t < triangleIdxList->size(); t += 3)
			{
				m->addTriangle(
					triangleIdxList->at(t),
					triangleIdxList->at(t + 1),
					triangleIdxList->at(t + 2)
				);
			}
		}
		//p.second.
	}

	return toReturn;
}
