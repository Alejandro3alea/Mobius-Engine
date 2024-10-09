#pragma once
#include "Singleton.h"

#include <glm/glm.hpp>
#include <limits>

class RandomNumberGenerator 
{
	Singleton(RandomNumberGenerator)

public:
	void Initialize();

	int GetInt(const int min = 0, const int max = INT_MAX);
	bool GetBool();
	float GetFloat(const float min = 0.0f, const float max = FLT_MAX);

	glm::vec2 GetVec2(const glm::vec2& min, const glm::vec2& max);
	glm::vec3 GetVec3(const glm::vec3& min, const glm::vec3& max);
	glm::vec4 GetVec4(const glm::vec4& min, const glm::vec4& max);
};

#define RNG RandomNumberGenerator::Instance()
