#version 400 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoord;

// ------------------- UNIFORM -------------------
uniform vec3 uViewPos;


// --------------------- OUT ---------------------
out OUT_IN_VARIABLES {
	vec2 TexUV;
} vs_out;


void main()
{
    vs_out.TexUV = vec2(vTexCoord.x, 1.0 - vTexCoord.y);
    gl_Position = vec4(vPos, 1.0);
}