#pragma once
#include "Billboard.h"
#include <pugixml.hpp>

// TODO OPCIONAL: Optimizaciones
class Particle :
	public Billboard
{
private:
	std::string name;
	glm::vec3 vel;
	double lifeTime;
	double maxLifeTime;
	bool autoFade;

public:
	Particle(
		std::string mshFile,
		const glm::vec3& vel,
		float spinVel,
		double lifetime,
		bool autofade
	);
	Particle(
		std::string mshFile,
		const glm::vec3& vel,
		const glm::vec3& pos,
		float spinVel,
		float scale,
		double lifetime,
		bool autofade
	);

	float getRemainingLifetime() const;
	virtual void step(float deltaTime) override;
};
