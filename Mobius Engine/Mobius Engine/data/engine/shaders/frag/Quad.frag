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
	vec3 color = texture(uTexture, fs_in.TexUV).rgb;
	FragColor = vec4(color, 1.0);
}