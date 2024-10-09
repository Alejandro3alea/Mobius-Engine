#include "ComputeShader.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Texture.h"
#include "Misc/ColorConsole.h"

#include <fstream>


ComputeShader::ComputeShader(const std::string& shaderFile)
{
	std::ifstream tFile(shaderFile);

	// parse shader file
	std::stringstream shaderCode;
	char line[256];
	while (tFile.getline(line, 256))
	{
		shaderCode << line << std::endl;
	}

	std::string finalCode = shaderCode.str();
	const char* strFileData = finalCode.c_str();

	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &strFileData, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		PrintError(std::string("Compile failure in compute shader:\n") + strInfoLog);
		delete[] strInfoLog;
	}
	else
	{
		PrintSuccess("Compute shader " + shaderFile + " successfully read.");
	}

	mID = glCreateProgram();
	glAttachShader(mID, shader);
	glLinkProgram(mID);

	glDeleteShader(shader);
}

ComputeShader::~ComputeShader()
{
	glDeleteProgram(mID);
}

void ComputeShader::Dispatch(const glm::uvec3& workgroups)
{
	glDispatchCompute(workgroups.x, workgroups.y, workgroups.z); 
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void ComputeShader::Dispatch(const unsigned workX, const unsigned workY, const unsigned workZ)
{
	glDispatchCompute(workX, workY, workZ);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}
