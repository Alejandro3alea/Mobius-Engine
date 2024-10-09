#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 90) out;

#define LIGHT_NUM_MAX 7

in VS_OUT 
{
	vec2 TexUV;
	vec3 Normal;
	vec3 Tangent;
	vec3 Bitangent;
} gs_in[];

out GS_OUT {
    vec2 TexUV;
	vec3 Normal;
    flat int ShellLayer; 
} gs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform int uShellCount;
uniform int uShellOffset;
uniform float uDensity;
uniform float uShellLength;
uniform float uDistAtt;
uniform float uCurvature;
uniform float uDisplacementStrength;
uniform sampler2D uTexture;

void main() 
{
    int maxDraws = min(uShellCount, uShellOffset + 30);
    for (int i = uShellOffset; i < maxDraws; i++)
    {
        float idx = i;
        float shellHeight = idx / uShellCount;
        
        for (int vtx = 0; vtx < 3; vtx++)
        {
            vec2 tempUV = floor(gs_in[vtx].TexUV * uDensity) / uDensity;
            float texValue = texture(uTexture, tempUV).r;
            float h = 1.0 + (pow(shellHeight, uDistAtt) * uShellLength);

            gl_Position = vec4(gl_in[vtx].gl_Position.x * h, gl_in[vtx].gl_Position.y * h, gl_in[vtx].gl_Position.z * h, gl_in[vtx].gl_Position.w);
            gl_Position = proj * view * model * gl_Position;
            gs_out.TexUV = tempUV;
            gs_out.Normal = gs_in[vtx].Normal;
            gs_out.ShellLayer = i;
            EmitVertex();
        }

        EndPrimitive();
    }
}