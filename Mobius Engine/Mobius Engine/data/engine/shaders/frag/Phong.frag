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

	for (int i = 0; i < uLightCount; i++)
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

void main()
{
	vec3 diffuseTexel = vec3(1,1,1);
	vec3 specularTexel = vec3(1,1,1);
	vec3 normalTexel = vec3(0.5, 0.5, 1.0);
	normalTexel = normalize(normalTexel * 2.0f - 1.0f);

	vec3 finalColor = vec3(0.0f, 0.0f, 0.0f);

	// ambient
	vec3 ambient = 0.1 * diffuseTexel;

	for (int i = 0; i < uLightCount; i++)
	{
		// diffuse 
		vec3 lightDir = normalize(fs_in.TanLightPos[i] - fs_in.TanFragPos);
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
}