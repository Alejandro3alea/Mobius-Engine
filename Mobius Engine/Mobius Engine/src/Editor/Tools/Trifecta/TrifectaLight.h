#pragma once
#include <glm/glm.hpp>

struct TrifectaLight
{
	TrifectaLight();

public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	glm::vec3 position;

	float spot_falloff;
	float spot_innerAngle;
	float spot_outerAngle;

	float const_Att;
	float linear_Att;
	float quadratic_Att;
};