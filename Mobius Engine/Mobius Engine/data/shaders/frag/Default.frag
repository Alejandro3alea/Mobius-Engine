#version 400 core

#define PI 3.141521

#define LIGHT_NUM_MAX 7

struct Light 
{
    vec3 Pos;
    vec3 Color;
};

struct Material
{
	vec3 Color;

	sampler2D Albedo;
	sampler2D Normal;
	sampler2D Metallic;
	sampler2D Roughness;
	sampler2D Oclussion;
	sampler2D Emissive;
};

// --------------------- IN ---------------------
in OUT_IN_VARIABLES 
{
	vec2 TexUV;
	vec3 FragPos;
	vec3 TanLightPos[LIGHT_NUM_MAX];
	vec3 TanViewPos;
	vec3 TanFragPos;
	vec3 Normal;
} fs_in;

// --------------------- OUT --------------------
out vec4 FragColor;

// ------------------- UNIFORM ------------------
uniform Material uMaterial;
uniform Light uLight[LIGHT_NUM_MAX];
uniform int uLightCount;

uniform vec3 uViewPos;

/*void main()
{
	vec3 diffuseTexel = texture(uMaterial.Albedo, fs_in.TexUV).rgb;
	vec3 specularTexel = texture(uMaterial.Metallic, fs_in.TexUV).rgb;
	vec3 normalTexel = texture(uMaterial.Normal, fs_in.TexUV).rgb;
	normalTexel = normalize(normalTexel * 2.0f - 1.0f);

	vec3 finalColor = vec3(0.0f, 0.0f, 0.0f);

	// ambient
	vec3 ambient = 0.1 * diffuseTexel;

	for (int i = 0; i < uLightNum; i++)
	{
		// diffuse 
		vec3 lightDir = normalize(fs_in.TanLightPos[i] - fs_in.FragPos);
		float diff = max(dot(lightDir, normalTexel), 0.0f);
		vec3 diffuse = uLight[i].Color * diff * diffuseTexel;  
		
		// specular
		vec3 viewDir = normalize(fs_in.TanViewPos - fs_in.FragPos);
		vec3 reflectDir = reflect(-lightDir, normalTexel);  
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normalTexel, halfwayDir), 0.0f), 32.0f); // @TODO: Shinyness
		vec3 specular = uLight[i].Color * spec * specularTexel;  
		
		// @TODO: Light casting types  
		    
		finalColor += diffuse + specular;
	}
	
	FragColor = vec4(ambient + finalColor, 1.0f);
}*/


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
void main()
{		
    vec3 albedo     = pow(texture(uMaterial.Albedo, fs_in.TexUV).rgb, vec3(2.2));
    float metallic  = texture(uMaterial.Metallic, fs_in.TexUV).r;
    float roughness = texture(uMaterial.Roughness, fs_in.TexUV).r;
    float ao        = texture(uMaterial.Oclussion, fs_in.TexUV).r;
	/*vec3 normalTexel = texture(uMaterial.Normal, fs_in.TexUV).rgb;
	normalTexel = normalize(normalTexel * 2.0f - 1.0f);*/

    vec3 N = fs_in.Normal;
    vec3 V = normalize(uViewPos - fs_in.FragPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < uLightCount; ++i) 
    {
        /*
        vec3 lightDir = normalize(fs_in.TanLightPos[i] - fs_in.FragPos);
		float diff = max(dot(lightDir, normalTexel), 0.0f);
		vec3 diffuse = uLight[i].Color * diff * diffuseTexel;  
		
		// specular
		vec3 viewDir = normalize(fs_in.TanViewPos - fs_in.FragPos);
		vec3 reflectDir = reflect(-lightDir, normalTexel);  
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normalTexel, halfwayDir), 0.0f), 32.0f); // @TODO: Shinyness
		vec3 specular = uLight[i].Color * spec * specularTexel;  */

        // calculate per-light radiance
        vec3 L = normalize(uLight[i].Pos - fs_in.FragPos);
        vec3 H = normalize(V + L);
        float distance = length(uLight[i].Pos - fs_in.FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = uLight[i].Color * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}