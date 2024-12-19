#include "Entity.h"

void Entity::computeModelMatrix()
{
	glm::mat4 model = glm::translate(glm::mat4(1.0), pos);
	model = glm::rotate(model, rot.x, glm::vec3(1, 0, 0));
	model = glm::rotate(model, rot.y, glm::vec3(0, 1, 0));
	model = glm::rotate(model, rot.z, glm::vec3(0, 0, 1));
	model = glm::scale(model, size);
	modelM = model;
}

glm::vec3 Entity::getPos()
{
	return pos;
}

glm::vec3 Entity::getRot()
{
	return rot;
}

glm::vec3 Entity::getSize()
{
	return size;
}

glm::mat4 Entity::getModelMatrix()
{
	return modelM;
}

void Entity::setRot(glm::vec3 _rot)
{
	rot = _rot;
}

void Entity::setSize(glm::vec3 _size)
{
	size = _size;
}

void Entity::setModelMatrix(glm::mat4 _model)
{
	modelM = _model;
}

void Entity::setPos(glm::vec3 _pos)
{
	pos = _pos;
}
