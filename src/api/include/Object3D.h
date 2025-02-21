#pragma once
#include "Object.h"
#include "Camera.h"
#include "Armature.h"
#include <pugixml.hpp>

struct MSH_t
{
	int numberOfUses;
	Mesh3D* mesh;
	size_t vertexCount;
	size_t idxCount;
};

class MeshManager
{
private:
	static inline std::map<std::string, MSH_t*> mshList;

public:
	static MSH_t* getMSH(std::string name);
	static void addMSH(std::string name, Mesh3D* msh);
};

class Object3D : public Object
{
protected:
	int fps;
	bool animate;
	int numFrames;
	int currentFrame;
	float timeAccum = 0;

	//virtual void step(double deltaTime) = 0;

	void loadTextures(pugi::xml_node textureNode, Material* mat);
	Material* getMeshMaterial(pugi::xml_node materialDescription);
	std::vector<glm::vec4>* getMeshVertexList(pugi::xml_node vertexListDescription, int vertexCompCount);
	std::vector<glm::vec2>* getMeshTexCoordList(pugi::xml_node vertexListDescription);
	std::vector<glm::uint32>* getMeshIndexList(pugi::xml_node indexListDescription);

	virtual void loadDataFromFile(std::string file) override;
	void loadDataFromFile(std::string file, bool sharedMesh) ;
	void loadObj(std::string objFile, Material* mat, std::string mshFileName);
	void loadObj(std::string objFile, Material* mat, std::string mshFileName, bool sharedMesh);
	void loadMsh(std::string fileName);
	void loadMsh(std::string fileName, bool sharedMesh);

public:
	Armature* armature = nullptr;
	Object3D() {};
	Object3D(std::string fileName);

	void setAnimate(bool animate);
	bool shouldAnimate();

	int getFps() const;
	void setFps(int fps);

	int getCurrentFrame() const;
	void setCurrentFrame(int frame);

	void calculateAnimMatrices();
	std::vector<glm::mat4>* getAnimMatrices();

	void step(float deltaTime) override;
};

class TrianguloRot : public Object3D {

public:
	TrianguloRot();

	void step(float deltaTime) override;

};

class CubeTex : public Object3D {
public:

	CubeTex();
	CubeTex(std::string fileName);
	void step(float deltaTime) override;
};

class FollowCamObject : public Object3D
{
public:
	FollowCamObject() {};
	FollowCamObject(std::string fileName) : Object3D(fileName) {}
	void step(float deltaTime) override;
};

class Mirror : public Object3D
{
public:
	Mirror() {};
	Mirror(int renderStep, Camera* mirrorCamera, Camera* playerCamera);

	void step(float deltaTime) override;
private:
	Camera* mirrorCamera;
	Camera* playerCamera;
};

class PostProcessPlane : public Object3D
{
public:
	PostProcessPlane() {};
	PostProcessPlane(int renderStep, Camera* renderCamera);

	void step(float deltaTime) override;

private:
	Camera* renderCamera;
}; 

class BoxCloud : public Object3D
{
public:
	BoxCloud();

	void step(float deltaTime);
};