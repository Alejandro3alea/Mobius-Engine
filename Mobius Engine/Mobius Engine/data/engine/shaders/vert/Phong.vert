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
	vec3 TanLightPos[LIGHT_NUM_MAX];
	vec3 TanViewPos;
	vec3 TanFragPos;
	vec3 Normal;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(model * vec4(vPos, 1.0));   
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalMatrix * vNormal;   
    vs_out.TexUV = vTexCoord;
    
    vec3 T = normalize(normalMatrix * vTangent);
    vec3 N = normalize(normalMatrix * vNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    
    for (int i = 0; i < uLightCount; i++)
        vs_out.TanLightPos[i] = TBN * uLight[i].Pos;

    vs_out.TanViewPos  = TBN * uViewPos;
    vs_out.TanFragPos  = TBN * vs_out.FragPos;

    gl_Position = proj * view * model * vec4(vPos, 1.0);
}