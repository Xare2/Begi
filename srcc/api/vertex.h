#pragma once
#include "common.h"

struct vertex_t 
{
	glm::vec4 pos;
	glm::vec4 color = glm::vec4(1);
	glm::vec4 normal;
	glm::vec2 texCoords;
	glm::vec4 tangent;
	glm::vec4 boneIdx;
	glm::vec4 boneWeights;
};