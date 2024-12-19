#include "Mesh3D.h"
#include "FactoryEngine.h"

Mesh3D::Mesh3D(Mesh3D& mesh)
{
	this->id = mesh.id;
	this->name = mesh.name;

	this->vVertList = new std::vector<vertex_t>(*mesh.vVertList);
	this->vTriangleIdxList = new std::vector<glm::uint32>(*mesh.vTriangleIdxList);

	this->mat = FactoryEngine::getNewMaterial();
	this->mat->copyValues(mesh.getMaterial());
}

Mesh3D::Mesh3D(std::string fileName)
{
	id = meshID++;
	mat = nullptr;
	vVertList = new std::vector<vertex_t>();
	vTriangleIdxList = new std::vector<glm::uint32>();
	this->name = fileName;
}

Mesh3D::Mesh3D(std::string fileName, std::vector<vertex_t>* vertexList_)
{
	id = meshID++;
	
	vVertList = vertexList_;
	vTriangleIdxList = new std::vector<glm::uint32>();

	mat = FactoryEngine::getNewMaterial();
	this->name = fileName;
}

Mesh3D::Mesh3D(std::string fileName, std::vector<vertex_t>* vertexList_, std::vector<glm::uint32>* vTriangleIdxList_)
{
	id = meshID++;

	vVertList = vertexList_;
	vTriangleIdxList = vTriangleIdxList_;

	mat = FactoryEngine::getNewMaterial();
	this->name = fileName;
}

int Mesh3D::getMeshID() const
{
	return id;
}

void Mesh3D::addVertex(vertex_t _vertex)
{
	vVertList->push_back(_vertex);
}

std::vector<vertex_t>* Mesh3D::getVertList() const
{
	return vVertList;
}

std::vector<glm::uint32_t>* Mesh3D::getTriangleIdxList() const
{
	return vTriangleIdxList;
}

void Mesh3D::setMaterial(Material* mat)
{
	this->mat = mat;
}

void Mesh3D::addId(int _id)
{
	this->vTriangleIdxList->push_back(_id);
}

void Mesh3D::recomputeNormals()
{
	for (auto& v : *this->vVertList)
		v.normal = { 0,0,0,0 };

	for (auto it = (*this->vTriangleIdxList).begin(); it != (*this->vTriangleIdxList).end();)
	{
		vertex_t& v1 = (*this->vVertList)[*it]; it++;
		vertex_t& v2 = (*this->vVertList)[*it]; it++;
		vertex_t& v3 = (*this->vVertList)[*it]; it++;
		//v1.normal = v2.normal = v3.normal = { 0,0,0,0 };
		glm::vec3 l1 = glm::normalize(v2.pos - v1.pos);
		glm::vec3 l2 = glm::normalize(v2.pos - v3.pos);

		glm::vec3 norm = glm::normalize(glm::cross(l2, l1));
		v1.normal = glm::normalize(v1.normal + glm::vec4(norm, 0.0f));
		v2.normal = glm::normalize(v2.normal + glm::vec4(norm, 0.0f));
		v3.normal = glm::normalize(v3.normal + glm::vec4(norm, 0.0f));
	}
}

std::string Mesh3D::getName() const
{
	return this->name;
}

void Mesh3D::setVertex(vertex_t _vertex, int pos)
{
	//std::cout << "value before: " << vVertList->at(pos).pos[0] << std::endl;
	(*vVertList)[pos] = _vertex;
	//std::cout << "value after: " << vVertList->at(pos).pos[0] << std::endl;
}

void Mesh3D::addTriangle(glm::uint32 vId1, glm::uint32 vId2, glm::uint32 vId3)
{
	vTriangleIdxList->insert(vTriangleIdxList->end(), { vId1, vId2, vId3 });
}

Material* Mesh3D::getMaterial() const
{
	return mat;
}
