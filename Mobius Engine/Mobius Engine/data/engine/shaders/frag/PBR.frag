#version 400 core

#define PI 3.141521
#define EPSILON 0.0001
#define MAX_REFLECTION_LOD 4.0

#define LIGHT_NUM_MAX 7

// ------------------- STRUCTS ------------------
struct Light 
{
    vec3 Pos;
    vec3 Color;
};

struct Material
{
	vec3 Color;
	float Ambient;
	float Metallic;
	float Roughness;

	sampler2D AlbedoTex;
	sampler2D NormalTex;
	sampler2D MetallicTex;
	sampler2D RoughnessTex;
	sampler2D OclussionTex;
	sampler2D EmissiveTex;

	int UseAlbedoTex;
	int UseNormalTex;
	int UseMetallicTex;
	int UseRoughnessTex;
	int UseOclussionTex;
	int UseEmissiveTex;
};

// --------------------- IN ---------------------
in OUT_IN_VARIABLES 
{
	vec2 TexUV;
	vec3 FragPos;
	vec3 Normal;
	vec3 Tangent;
	vec3 Bitangent;
} fs_in;

// --------------------- OUT --------------------
out vec4 FragColor;

// ------------------- UNIFORM ------------------
uniform Material uMaterial;
uniform Light uLight[LIGHT_NUM_MAX];
uniform int uLightCount;

uniform float ao;
uniform float metallic;
uniform float roughness;

uniform samplerCube uIrradianceMap;
uniform samplerCube uPrefilterMap;
uniform sampler2D uBRDFLUT;

uniform vec3 uViewPos;

// ----------------------------------------------------------------------------

float BRDF()
{
    return 0.0f;
}

// ----------------------------------------------------------------------------

vec3 LambertianDiffuse(vec3 color)
{
    return color / PI;
}

// ----------------------------------------------------------------------------

float NormalDistributionFunction(const float NdotH, const float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float denomVal = NdotH * NdotH * (a2 - 1.0) + 1.0;

    return a2 / (PI * denomVal * denomVal);
}

// ----------------------------------------------------------------------------

float GetKDirect(const float roughness)
{
    float numVal = roughness + 1.0;
    return numVal * numVal / 8.0;
}

// ----------------------------------------------------------------------------

float GetKIBL(const float roughness)
{
    return roughness * roughness / 2.0f;
}

// ----------------------------------------------------------------------------

float GeometrySchlick(const float dotVal, const float k)
{
    return dotVal / (dotVal * (1.0 - k) + k);
}

// ----------------------------------------------------------------------------

float GeometrySmith(const float NdotV, const float NdotL, const float k)
{
    return GeometrySchlick(NdotV, k) * GeometrySchlick(NdotL, k);
}

// ----------------------------------------------------------------------------

vec3 FresnelShlick(const float HdotV, const vec3 normalIncidence)
{
    return normalIncidence + (1.0 - normalIncidence) * pow(1.0 - HdotV, 5.0);
}

// ----------------------------------------------------------------------------

vec3 FresnelShlickRoughness(const float HdotV, const vec3 normalIncidence, float roughness)
{
    return normalIncidence + (max(vec3(1.0 - roughness), normalIncidence) - normalIncidence) * max(pow(1.0 - HdotV, 5.0), 0.0);
}

// ----------------------------------------------------------------------------

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(uMaterial.NormalTex, fs_in.TexUV).xyz * 2.0 - 1.0;
    mat3 TBN = mat3(fs_in.Tangent, fs_in.Bitangent, fs_in.Normal);
    return normalize(TBN * tangentNormal);
}

// ----------------------------------------------------------------------------

void main()
{		
    vec3 albedo      = (uMaterial.UseAlbedoTex > 0) ? pow(texture(uMaterial.AlbedoTex, fs_in.TexUV).rgb, vec3(2.2)) : uMaterial.Color;
	vec3 normalTexel = (uMaterial.UseNormalTex > 0) ? GetNormalFromMap() : fs_in.Normal;
    float metallic   = (uMaterial.UseMetallicTex > 0) ? texture(uMaterial.MetallicTex, fs_in.TexUV).r : uMaterial.Metallic;
    float roughness  = (uMaterial.UseRoughnessTex > 0) ? texture(uMaterial.RoughnessTex, fs_in.TexUV).r : uMaterial.Roughness;
    float ao         = (uMaterial.UseOclussionTex > 0) ? texture(uMaterial.OclussionTex, fs_in.TexUV).r : uMaterial.Ambient;
    
    float geometry_r = roughness + 1.0;
    float geometry_k = geometry_r * geometry_r / 8.0;
    
    const vec3 dielectricIncidence = vec3(0.04); 
    vec3 normalIncidence = mix(dielectricIncidence, albedo, metallic);

    vec3 viewDir = normalize(uViewPos - fs_in.FragPos);
    float NdotV = max(dot(normalTexel, viewDir), 0.0);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < uLightCount; ++i) 
    {
        vec3 lightDir = normalize(uLight[i].Pos - fs_in.FragPos);
        vec3 halfwayVec = normalize(viewDir + lightDir);
        
        float NdotL = max(dot(normalTexel, lightDir), 0.0);
        float NdotH = max(dot(normalTexel, halfwayVec), 0.0);
        float HdotV = max(dot(halfwayVec, viewDir), 0.0);

        // Used for point lights
        float distFromLight = length(uLight[i].Pos - fs_in.FragPos);
        float attenuation = 1.0 / (distFromLight * distFromLight);
        vec3 radiance = uLight[i].Color * attenuation;

        // Cook-Torrance BRDF
        float D = NormalDistributionFunction(NdotH, roughness);   
        float G = GeometrySmith(NdotV, NdotL, geometry_k);      
        vec3  F = FresnelShlick(HdotV, normalIncidence);

        vec3 specular = D * G * F / (4.0 * NdotV * NdotL + EPSILON);

        vec3 Kd = (vec3(1.0) - F) * (1.0 - metallic);	  

        Lo += (Kd * albedo / PI + specular) * radiance * NdotL; 
    }

    vec3 kS = FresnelShlickRoughness(max(dot(normalTexel, viewDir), 0.0), normalIncidence, roughness);
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    vec3 irradiance = texture(uIrradianceMap, normalTexel).rgb;
    vec3 diffuse = irradiance * albedo;
    
    vec3 prefilteredColor = textureLod(uPrefilterMap, reflect(-viewDir, normalTexel), roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(uBRDFLUT, vec2(NdotV, roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    vec3 finalColor = ambient + Lo;

    FragColor = vec4(finalColor, 1.0);

}