#pragma once
#include "Shader.h"

struct ComputeShader : public ShaderBase
{
	ComputeShader(const std::string& shaderFile);
	~ComputeShader();

	void Dispatch(const glm::uvec3& workgroups);
	void Dispatch(const unsigned workX, const unsigned workY, const unsigned workZ);
};