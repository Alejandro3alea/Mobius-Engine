#version 400 core

#define PI 3.141521

#define LIGHT_NUM_MAX 7

struct Light 
{
    vec3 Pos;
    vec3 Color;
};

// --------------------- IN ---------------------
in GS_OUT {
    vec2 TexUV;
	vec3 Normal;
    flat int ShellLayer; 
} fs_in;

// --------------------- OUT --------------------
out vec4 FragColor;

// ------------------- UNIFORM ------------------
uniform Light uLight[LIGHT_NUM_MAX];
uniform int uLightCount;

uniform int uShellCount;
uniform float uDensity;
uniform float uNoiseMin;
uniform float uNoiseMax;
uniform float uThickness;
uniform float uOcclusionAtt;
uniform float uOcclusionBias;
uniform vec3 uShellColor;

uniform vec3 uViewPos;

float hash(uint n) 
{
	// integer hash copied from Hugo Elias
	n = (n << 13U) ^ n;
	n = n * (n * n * 15731U + 0x789221U) + 0x17631589U;
	return float(n & uint(0x7fffffffU)) / float(0x7fffffff);
}

void main()
{		
    vec2 newUV = fs_in.TexUV * uDensity;
    vec2 localUV = fract(newUV) * 2 - 1;
	
	float localDistanceFromCenter = length(localUV);
    uvec2 tid = uvec2(newUV);
    uint seed = tid.x + 100 * tid.y + 100 * 10;
    
    float rand = mix(uNoiseMin, uNoiseMax, hash(seed));

    float layer = fs_in.ShellLayer;
    float h = layer / uShellCount;

    bool outsideThickness = (localDistanceFromCenter) > (uThickness * (rand - h));
    
    if (outsideThickness && fs_in.ShellLayer > 0)
    {
        discard;
    }

    vec3 _WorldSpaceLightPos0 = vec3(1.0, 2.0, -2.0);
    float ndotl = clamp(dot(fs_in.Normal, _WorldSpaceLightPos0), 0.0, 1.0) * 0.5f + 0.5f;
    
    //ndotl = ndotl * ndotl;
    
    float ambientOcclusion = pow(h, uOcclusionAtt);

    ambientOcclusion += uOcclusionBias;

    ambientOcclusion = clamp(ambientOcclusion, 0.0, 1.0);
    
    FragColor = vec4(uShellColor * ndotl * ambientOcclusion, !outsideThickness || fs_in.ShellLayer == 0);
}