#version 400 core

#define PI 3.141521
#define EPSILON 0.0001

// --------------------- IN ---------------------
in vec3 texUV;

// --------------------- OUT --------------------
out vec4 FragColor;

uniform samplerCube uSkybox;

void main()
{
    vec3 N = normalize(texUV);

    vec3 totalIrradiance = vec3(0.0);   
    
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));
       
    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            totalIrradiance += texture(uSkybox, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    totalIrradiance = PI * totalIrradiance / (float(nrSamples) + EPSILON);
    
    FragColor = vec4(totalIrradiance, 1.0);
}