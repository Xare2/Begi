#include "Object.h"

Mesh3D* Object::getMesh(int pos)
{
	return meshList[pos];
}

std::vector<Mesh3D*> Object::getMeshes()
{
	return meshList;
}

void Object::addMesh(Mesh3D* _mesh)
{
	meshList.push_back(_mesh);
}

void Object::recomputeNormals()
{
	for (auto& m : this->meshList)
	{
		//for (auto& v : m->getVertList())
		for (size_t i = 0; i < m->getVertList()->size(); i++)
			m->getVertList()->at(i).normal = { 0,0,0,0 };

		//for (auto it = m->getTriangleIdxList().begin(); it != m->getTriangleIdxList().end();)
		for (size_t i = 0; i < m->getTriangleIdxList()->size(); i += 3)
		{
			vertex_t& v1 = m->getVertList()->at(i);
			vertex_t& v2 = m->getVertList()->at(i + 1);
			vertex_t& v3 = m->getVertList()->at(i + 2);
			//v1.normal = v2.normal = v3.normal = { 0,0,0,0 };
			glm::vec3 l1 = glm::normalize(v2.pos - v1.pos);
			glm::vec3 l2 = glm::normalize(v2.pos - v3.pos);

			glm::vec3 norm = glm::normalize(glm::cross(l2, l1));
			v1.normal = glm::normalize(v1.normal + glm::vec4(norm, 0.0f));
			v2.normal = glm::normalize(v2.normal + glm::vec4(norm, 0.0f));
			v3.normal = glm::normalize(v3.normal + glm::vec4(norm, 0.0f));
		}
	}
}

Object::~Object()
{
	//TODO eliminar la mesh delete mesh.clear();
}
