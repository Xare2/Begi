#pragma once
#include "common.h"
#include <glm/gtx/quaternion.hpp>
#include <pugixml.hpp>

using namespace glm;
class Bone {
public:
	int id = 0;

	inline static int idCounter = 0;

	bool updated = false;
	std::string name = "";
	glm::mat4 invPos = mat4(1.0f);
	glm::mat4 boneMatrix = mat4(1.0f);

	std::map<int, glm::vec3> positions;
	std::map<int, glm::quat> rotations;
	std::map<int, glm::vec3> scales;
	Bone* parent = nullptr;

	Bone(std::string name, Bone* parent) : name(name), parent(parent)
	{
		id = idCounter;
		idCounter++;
	}

// #define	computeFrame(dato, frame, vector,method)\
// 	{\
// 		if (vector.size() > 0)  {\
// 			auto it = vector.begin();\
// 			auto prevIt = vector.begin();\
// 			while (it != vector.end() && it->first < frame)\
// 			{\
// 				prevIt = it;\
// 				it++;\
// 			}\
// 			\
// 			if (it == vector.end())\
// 			{\
// 				dato = prevIt->second;\
// 			}\
// 			else {\
// 				if(it->first==frame)\
// 				{\
// 					dato = it->second; \
// 				}\
// 				else {\
// 					float rate = (float)(frame - prevIt->first) / (float)(it->first - prevIt->first); \
// 						dato = ##method(prevIt->second, it->second, rate); \
// 				}\
// 			}\
// 		}\
// 	}\



	void updateParent()
	{
		if (updated || !parent)
		{
			return;
		}
		if (!parent->updated)
		{
			parent->updateParent();
		}
		this->boneMatrix = parent->boneMatrix * this->boneMatrix;
		this->updated = true;
	}

	void update(int frame) {
		//calcular vectores position, rotation,scaling en frame
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scaling;

		// computeFrame(position, frame, positions, mix);
		// computeFrame(rotation, frame, rotations, slerp);
		// computeFrame(scaling, frame, scales, mix);

		//calcular matrices transformacion
		glm::mat4 t = glm::translate(glm::mat4(1.0), position);
		glm::mat4 r = glm::toMat4(rotation);
		glm::mat4 s = glm::scale(glm::mat4(1.0), scaling);
		// 
		//multiplicarlas
		this->boneMatrix = t * r * s;
	}

};


class Armature {
public:
	std::map<std::string, Bone*> nameList;
	std::vector<Bone*> idList;
	Armature() {};
	void loadArmature(pugi::xml_node boneBuffer) {

		Bone::idCounter = 0;

		for (pugi::xml_node boneNode = boneBuffer.child("bone");
			boneNode;
			boneNode = boneNode.next_sibling("bone"))
		{
			Bone* b;
			Bone* parent = nullptr;
			if (boneNode.child("parent"))
			{
				parent = nameList[boneNode.child("parent").text().as_string()];
			}
			b = new Bone(boneNode.child("name").text().as_string(), parent);

			std::vector<float> invPos = splitString<float>(boneNode.child("invPose").text().as_string(), ',');
			b->invPos = glm::make_mat4(invPos.data());

			std::vector<float> positions = splitString<float>(boneNode.child("positions").text().as_string(), ',');
			std::vector<float> rotations = splitString<float>(boneNode.child("rotations").text().as_string(), ',');
			std::vector<float> scalings = splitString<float>(boneNode.child("scales").text().as_string(), ',');

			auto pos = positions.begin();
			auto rot = rotations.begin();
			auto scal = scalings.begin();

			while (pos != positions.end())
			{
				int frameId = (int)*pos++;
				glm::vec3 position;
				position.x = *pos++;
				position.y = *pos++;
				position.z = *pos++;
				b->positions[frameId] = position;
			}
			while (rot != rotations.end())
			{
				int frameId = (int)*rot++;
				glm::quat position;
				position.w = *rot++;
				position.x = *rot++;
				position.y = *rot++;
				position.z = *rot++;
				b->rotations[frameId] = position;
			}
			while (scal != scalings.end())
			{
				int frameId = (int)*scal++;
				glm::vec3 position;
				position.x = *scal++;
				position.y = *scal++;
				position.z = *scal++;
				b->scales[frameId] = position;
			}

			nameList[b->name] = b;
			idList.push_back(b);
		}
	}

	void updateBones(int frame)
	{
		//calcular matriz por cada hueso
		for (auto b : idList)
		{
			b->updated = false;
			b->update(frame);
		}
		//actualizar matriz por cada parent
		for (auto b : idList)
		{
			b->updateParent();
		}

		//multiplica cada hueso por su matriz inversa
		for (auto b : idList)
		{
			b->boneMatrix = b->boneMatrix * b->invPos;
		}
	}
};