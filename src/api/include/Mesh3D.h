#pragma once
#include "common.h"
#include "vertex.h"
#include "Material.h"

class Mesh3D
{
private:
	
	std::vector<vertex_t>* vVertList;
	std::vector<glm::uint32>* vTriangleIdxList;
	Material* mat;
	std::string name;
	int id;

	//std::vector<unsigned int> idList;

public:
	static inline int meshID = 0;

	glm::vec4 colorRGBA = { 1.0f, 1.0f , 1.0f , 1.0f };

	Mesh3D(Mesh3D& mesh);
	Mesh3D(std::string fileName);
	Mesh3D(std::string fileName, std::vector<vertex_t>* vertexList_);
	Mesh3D(std::string fileName, std::vector<vertex_t>* vertexList_, std::vector<glm::uint32>* vTriangleIdxList_);
	
	int getMeshID() const;
	void addVertex(vertex_t _vertex);
	void addTriangle(glm::uint32 vId1, glm::uint32 vId2, glm::uint32 vId3);

	std::vector<vertex_t>* getVertList() const;
	std::vector<glm::uint32>* getTriangleIdxList() const;

	void setVertex(vertex_t _vertex, int pos);

	Material* getMaterial() const;
	void setMaterial(Material* material);

	void addId(int _id);

	void recomputeNormals();

	std::string getName() const;
};

