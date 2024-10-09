#include "TrifectaLight.h"

TrifectaLight::TrifectaLight()
{
	ambient = { 0.05f, 0.05f, 0.05f };
	diffuse = { 1.0f, 1.0f, 1.0f };
	specular = { 1.0f, 1.0f, 1.0f };

	position = { -7.0f, 5.0f, 10.0f };

	spot_falloff = 1.0f;
	spot_innerAngle = 30.0f;
	spot_outerAngle = 40.0f;

	const_Att = 0.0f;
	linear_Att = 0.001f;
	quadratic_Att = 0.001f;
}
