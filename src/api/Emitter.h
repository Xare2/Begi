#pragma once
#include "Entity.h"
#include "Particle.h"

class Emitter :
    public Entity
{
private:
    bool emit;
    bool fade;
    std::string mshFile;
    float numParticulasNuevas = 0;

    float minRate;
    float maxRate;

    float minSpin;
    float maxSpin;

    float minScale;
    float maxScale;

    float minLifetime;
    float maxLifetime;

    glm::vec3 minVel;
    glm::vec3 maxVel;

    glm::vec4 minColor;
    glm::vec4 maxColor;

    std::map<float, Particle*>* particleList;

public:
    Emitter(const std::string mshFile, float emissionRate, bool autofade);

    void setRateRange(float min, float max);
    void setVelocityRange(const glm::vec3& min, const glm::vec3& max);
    void setSpinVelocityRange(float min, float max);
    void setScaleRange(float min, float max);
    void setLifetimeRange(float min, float max);
    void setColorRange(const glm::vec4& min, const glm::vec4& max);

    void setEmit(bool enable);
    bool isEmitting();

    std::map<float, Particle*>* getParticleList();
    virtual void step(float deltaTime) override;

    Object* getAllParticlesObject() const;
};
