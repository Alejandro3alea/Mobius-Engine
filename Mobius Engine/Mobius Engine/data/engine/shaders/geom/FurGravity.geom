#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 90) out;

#define LIGHT_NUM_MAX 7

in VS_OUT 
{
	vec2 TexUV;
	vec3 Normal;
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
uniform float uShellLength;
uniform float uDistAtt;
uniform float uCurvature;
uniform float uDisplacementStrength;
uniform vec3 uSpringPos;

float smin(float a, float b, float t)
{
    float h = max(t - abs(a-b), 0.0) / t;
    return min(a,b) - h*h*t/4.0;
}

void main() 
{
    int maxDraws = min(uShellCount, uShellOffset + 30);
    for (int i = uShellOffset; i < maxDraws; i++)
    {
        float idx = i;
        float shellHeight = idx / uShellCount;
        
        float k = pow(shellHeight, uCurvature);
        float h = 1.0 + (pow(shellHeight, uDistAtt) * uShellLength);

        for (int vtx = 0; vtx < 3; vtx++)
        {
            vec3 dirVec = (uSpringPos + gs_in[vtx].Normal * h) * k * uDisplacementStrength;
            float intersection = dot(gs_in[vtx].Normal, dirVec);
            vec3 correctedDir = dirVec - gs_in[vtx].Normal * smin(intersection, -uShellLength / 10 * shellHeight, 0.1);

           /* float springLen = smin(length(correctedDir), 0.1, 1.0);
            correctedDir = springLen * normalize(correctedDir);
            */
            //while (dot(dirVec, gs_in[vtx].Normal) < 0.0)
                //dirVec += gs_in[vtx].Normal;

            
            gl_Position = vec4(dirVec, 0.0) + vec4(gl_in[vtx].gl_Position.x, gl_in[vtx].gl_Position.y, gl_in[vtx].gl_Position.z, gl_in[vtx].gl_Position.w);
            gl_Position = proj * view * model * gl_Position;
            gs_out.TexUV = gs_in[vtx].TexUV;
            gs_out.Normal = gs_in[vtx].Normal;
            gs_out.ShellLayer = i;
            EmitVertex();
        }

        EndPrimitive();
    }
}