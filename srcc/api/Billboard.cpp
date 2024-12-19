#include "Billboard.h"
#include "System.h"

Billboard::Billboard()
{
    this->spin = 0;
}

float Billboard::getSpin() const
{
    return this->spin;
}

void Billboard::setSpin(float spin)
{
    this->spin = spin;
}

void Billboard::computeModelMatrix()
{
    glm::mat4 cam = glm::transpose(
        System::getWorld()->getCamera(System::getWorld()->getActiveCamera())->getView()
    );
    cam[0][3] = 0;
    cam[1][3] = 0;
    cam[2][3] = 0;
    cam[3] = glm::vec4(this->getPos(), 1);

    cam = glm::scale(cam, this->getSize());

    this->setModelMatrix(cam);
}
