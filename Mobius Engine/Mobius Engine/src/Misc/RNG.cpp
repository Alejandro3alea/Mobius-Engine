#include "RNG.h"

#include <time.h>

RandomNumberGenerator* RandomNumberGenerator::mpInstance;

void RandomNumberGenerator::Initialize()
{
	srand((unsigned)time(0));
}

int RandomNumberGenerator::GetInt(const int min, const int max)
{
	return min + std::rand() % (max - min);
}

bool RandomNumberGenerator::GetBool()
{
	return static_cast<bool>(std::rand() % 2);
}

float RandomNumberGenerator::GetFloat(const float min, const float max)
{
	return min + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX / (max - min));
}

glm::vec2 RandomNumberGenerator::GetVec2(const glm::vec2& min, const glm::vec2& max)
{
	glm::vec2 result;
	result.x = GetFloat(min.x, max.x);
	result.y = GetFloat(min.y, max.y);
	return result;
}

glm::vec3 RandomNumberGenerator::GetVec3(const glm::vec3& min, const glm::vec3& max)
{
	glm::vec3 result;
	result.x = GetFloat(min.x, max.x);
	result.y = GetFloat(min.y, max.y);
	result.z = GetFloat(min.z, max.z);
	return result;
}

glm::vec4 RandomNumberGenerator::GetVec4(const glm::vec4& min, const glm::vec4& max)
{
	glm::vec4 result;
	result.x = GetFloat(min.x, max.x);
	result.y = GetFloat(min.y, max.y);
	result.z = GetFloat(min.z, max.z);
	result.w = GetFloat(min.w, max.w);
	return result;
}
