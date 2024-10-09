#version 400 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

#define LIGHT_NUM_MAX 7

struct Light
{
    vec3 Pos;
    vec3 Color;
};

// ------------------- UNIFORM -------------------
uniform Light uLight[LIGHT_NUM_MAX];
uniform int uLightCount;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 uViewPos;

// --------------------- OUT ---------------------
out OUT_IN_VARIABLES {
	vec2 TexUV;
	vec3 FragPos;
	vec3 Normal;
	vec3 Tangent;
	vec3 Bitangent;
} vs_out;

void main()
{

    vs_out.FragPos = vec3(model * vec4(vPos, 1.0));   
    mat3 modelMtx = mat3(model);
    mat3 normalMtx = transpose(inverse(modelMtx));
    vs_out.Normal = normalMtx * vNormal;   
    vs_out.Tangent = modelMtx * vTangent;   
    vs_out.Bitangent = modelMtx * vBitangent;   
    vs_out.TexUV = vTexCoord;
    
    gl_Position = proj * view * model * vec4(vPos, 1.0);
}