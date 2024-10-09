#pragma once
#include "PostProcessFx.h"
#include "Texture.h"
#include "Shader/ComputeShader.h"

struct Framebuffer;

struct BloomEffect : public PostProcessEffect
{
    BloomEffect(const std::string& shaderPath);

	virtual void Initialize();
	virtual void Render() override;


private:
    unsigned calculateMipmapLevels();

public:
    Resource<ComputeShader>* mUpscaleShader = nullptr;
    Resource<ComputeShader>* mDownscaleShader = nullptr;
    Resource<Texture>* mDirtMask = nullptr;

    glm::uvec2 mSize;

    uint8_t mMipmapCount = 1;
};