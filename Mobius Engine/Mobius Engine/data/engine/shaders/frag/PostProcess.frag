#version 460 core

// --------------------- IN ---------------------
in OUT_IN_VARIABLES 
{
	vec2 TexUV;
} fs_in;

// --------------------- OUT --------------------
out vec4 FragColor;


// ------------------- UNIFORM ------------------
uniform sampler2D uTexture;

uniform float uGamma;
uniform float uExposure;

void main()
{
    vec3 fbTexel = texture(uTexture, fs_in.TexUV).rgb;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-fbTexel * uExposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / uGamma));
  
    FragColor = vec4(mapped, 1.0);
}