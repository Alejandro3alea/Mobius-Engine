#version 400 core

#define NUM_VIEW_STEP_COUNT 7
#define NUM_OPTICAL_DEPTH_POINTS 7

// --------------------- IN ---------------------
in vec3 FragPos;

// --------------------- OUT --------------------
out vec4 FragColor;

// ------------------- UNIFORM ------------------
uniform vec3 uViewPos;
uniform vec3 uViewDir;

uniform vec3 uPlanetCenter;
uniform float uPlanetRadius;
uniform float uAtmosphereRadius;

uniform float uDensityFalloff;

// ----------------------------------------------------------------------------

float DensityAtPoint(vec3 point)
{
	float heightFromSurface = length(point - uPlanetCenter) - uPlanetRadius;
	float normalizedHeight = heightFromSurface / (uAtmosphereRadius - uPlanetRadius);
	float localDensity = exp(-normalizedHeight * uDensityFalloff) * (1.0 - normalizedHeight);
	return localDensity;
}

float OpticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLen)
{
	vec3 densitySamplePoint = rayOrigin;
	float stepSize = rayLen / (NUM_OPTICAL_DEPTH_POINTS - 1.0);

	float opticalDepth;
	for (int i = 0; i < NUM_OPTICAL_DEPTH_POINTS; i++)
	{
		float localDensity = DensityAtPoint(densitySamplePoint);
		opticalDepth += localDensity * stepSize;
		densitySamplePoint += rayDir * stepSize;
	}

	return opticalDepth;
}

float CalculateLight(vec3 rayOrigin, vec3 rayDir, float rayLen)
{
	float scatteredLight = 0.0;
	vec3 scatterPoint = rayOrigin;
	float stepSize = rayLen / (MAX_LIGHT_STEP_COUNT - 1.0);
	
	float sphereRadius = 1.0;
	vec3 sunPos = -rayDir * sphereRadius;

	for (int i = 0; i < MAX_LIGHT_STEP_COUNT; i++)
	{
		vec3 sunToPoint = scatterPoint - sunPos;
		float sunRayLen = len
		scatteredLight += OpticalDepth(sunPos, sunToPoint, );
		scatterPoint += rayDir * stepSize;
	}

	return inScatteredLight
}

// ----------------------------------------------------------------------------

void main()
{
	vec3 rayOrigin = uViewPos;
	vec3 rayDir = normalize(uViewDir);

	float dist to

    FragColor = vec4(1.0, 1.0, 1.0, 1.0);

}