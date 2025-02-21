#pragma once
#include "Mesh3D.h"
#include "Entity.h"

class Object : public Entity
{
private:
	std::vector<Mesh3D*> meshList;

public:
	unsigned int type;

	virtual void loadDataFromFile(std::string file) = 0;

	Mesh3D* getMesh(int pos);
	std::vector<Mesh3D*> getMeshes();
	void addMesh(Mesh3D* _mesh);
	void recomputeNormals();

	~Object();
};

