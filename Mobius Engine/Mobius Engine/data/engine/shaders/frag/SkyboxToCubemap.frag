#version 400 core

#define MAX_COLOR_VALUE 50000

// --------------------- IN ---------------------
in vec3 texUV;

// --------------------- OUT --------------------
out vec4 FragColor;

uniform sampler2D uHDR;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(texUV));
    vec3 color = min(texture(uHDR, uv).rgb, vec3(MAX_COLOR_VALUE));
    FragColor = vec4(color, 1.0);
}