#pragma once
#include "Object3D.h"
#include "common.h"

class Billboard :
    public Object3D
{
private:
    float spin;//velocidad de rotación en eje z
public:
    Billboard();

    float getSpin() const;
    void setSpin(float spin);

    virtual void computeModelMatrix() override;
    virtual void step(float deltaTime) override = 0;
};
