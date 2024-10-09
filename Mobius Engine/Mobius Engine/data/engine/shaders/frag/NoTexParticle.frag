#version 400 core

// --------------------- IN ---------------------
in OUT_IN_VARIABLES {
	vec2 TexUV;
} fs_in;

// --------------------- OUT --------------------
out vec4 FragColor;


// ------------------- UNIFORM ------------------
uniform sampler2D uTexture;


void main()
{
	vec2 UVpos = (fs_in.TexUV - vec2(0.5)) * 2.0;
	float radius = dot(UVpos, UVpos);

	if (radius > 1.0) 
		discard;

	FragColor = vec4(1.0, 1.0, 1.0, 1.0 - radius);
}