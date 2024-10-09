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
uniform int uShellOffset;
uniform float uDensity;
uniform float uNoiseMin;
uniform float uNoiseMax;
uniform float uThickness;
uniform float uOcclusionAtt;
uniform float uOcclusionBias;
uniform vec3 uShellColor;
uniform sampler2D uTexture;

uniform float uTime;
uniform vec3 uViewPos;

void main()
{		
    float layer = fs_in.ShellLayer;
    float h = layer / uShellCount;
    float offset = uTime / 100.0;
    vec2 newUV = floor(fs_in.TexUV * uDensity * offset) / uDensity;
    
    float texValue = texture(uTexture, newUV).r;
    float rand = mix(uNoiseMin, uNoiseMax, texValue);

    bool outsideThickness = rand < h;
    
    if (outsideThickness && fs_in.ShellLayer > 0)
    {
        discard;
    }

    vec3 _WorldSpaceLightPos0 = vec3(1.0, 2.0, -2.0);
    float ndotl = clamp(dot(fs_in.Normal, _WorldSpaceLightPos0), 0.0, 1.0) * 0.5f + 0.5f;
    
    ndotl = ndotl * ndotl;
    
    float ambientOcclusion = pow(h, uOcclusionAtt);

    ambientOcclusion += uOcclusionBias;

    ambientOcclusion = clamp(ambientOcclusion, 0.0, 1.0);
    
    /*vec3 tentacleColor = vec3(0.84, 0.75, 0.15);
    vec3 bodyColor = vec3(0.69, 0.153, 0.86);*/
    vec3 bodyColor = vec3(0.15, 0.0, 0.0);
    vec3 tentacleColor = vec3(0.99, 0.99, 0.99);
    //vec3 tentacleColor = vec3(0.05, 0.57, 0.03);
    vec3 texCol = mix(bodyColor, tentacleColor, ambientOcclusion);
    FragColor = vec4(texCol * ndotl, !outsideThickness || fs_in.ShellLayer == 0);
    //FragColor = vec4(newUV, 0.0, 1.0);
    //FragColor = vec4(outsideThickness, outsideThickness, outsideThickness, 1.0);
    //FragColor = vec4(texValue, texValue, texValue, 1.0);
}