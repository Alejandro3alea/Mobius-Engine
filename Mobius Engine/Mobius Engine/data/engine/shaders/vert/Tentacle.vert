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

uniform vec3 uViewPos;

// --------------------- OUT ---------------------
out VS_OUT {
	vec2 TexUV;
	vec3 Normal;
	vec3 Tangent;
	vec3 Bitangent;
} vs_out;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalMatrix * vNormal;
    vec3 T = normalize(normalMatrix * vTangent);
    vs_out.Tangent = normalize(T - dot(T, vs_out.Normal) * vs_out.Normal);
    vs_out.Bitangent = cross(vs_out.Normal, vs_out.Tangent);
    vs_out.TexUV = vTexCoord;
    gl_Position = vec4(vPos, 1.0);
}