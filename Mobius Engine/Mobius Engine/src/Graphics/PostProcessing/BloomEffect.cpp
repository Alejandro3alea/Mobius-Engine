#include "BloomEffect.h"
#include "Framebuffer.h"
#include "WindowMgr.h"
#include "GfxMgr.h"
#include "Debug/GLDebug.h"

BloomEffect::BloomEffect(const std::string& shaderPath) : PostProcessEffect(shaderPath)
{
}

unsigned BloomEffect::calculateMipmapLevels()
{
	const unsigned minSize = 10;

	unsigned width = mSize.x / 2;
	unsigned height = mSize.y / 2;
	unsigned mipLevels = 1;

	while (width > minSize && height > minSize)
	{
		width = width / 2;
		height = height / 2;

		++mipLevels;
	}

	return mipLevels;
}

void BloomEffect::Initialize()
{
	mDirtMask = ResourceMgr->Load<Texture>("data/engine/textures/bloom_dirt_mask.png");
	mDownscaleShader = ResourceMgr->Load<ComputeShader>("data/engine/shaders/comp/BloomDownscale.comp");
	mUpscaleShader = ResourceMgr->Load<ComputeShader>("data/engine/shaders/comp/BloomUpscale.comp");

	mSize = WindowMgr->mCurrentWindow->size();
	mMipmapCount = calculateMipmapLevels();
	mFb = std::shared_ptr<Framebuffer>(new Framebuffer(1, HDRBuffer(mSize)));
	mFb->Initialize();
}

void BloomEffect::Render()
{
	ComputeShader* downShader = mDownscaleShader->get();
	ComputeShader* upShader   = mUpscaleShader->get();

	downShader->Bind();
	downShader->SetUniform("uThreshold", glm::vec4(GfxMgr->mBloomThreshold, 
												   GfxMgr->mBloomThreshold - GfxMgr->mBloomKnee, 
												   2.0f * GfxMgr->mBloomKnee, 
												   0.25f * GfxMgr->mBloomKnee));

	glBindTextureUnit(0, mFb->GetTexID());

	glm::uvec2 mip_size = glm::uvec2(mSize.x / 2, mSize.y / 2);

	for (uint8_t i = 0; i < mMipmapCount - 1; ++i)
	{
		downShader->SetUniform("uTexelSize", 1.0f / glm::vec2(mip_size));
		downShader->SetUniform("uMipLevel", i);
		downShader->SetUniform("uUseThreshold", i == 0);

		glBindImageTexture(0, mFb->GetTexID(), i + 1, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mFb->GetTexID());

		downShader->Dispatch(max(mip_size.x / 8, 1), max(mip_size.y / 8, 1), 1);

		mip_size = mip_size / 2u;

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
	}

	// Bloom: upscale
	upShader->Bind();
	upShader->SetUniform("uBloomIntensity", GfxMgr->mBloomIntensity);
	upShader->SetUniform("uDirtIntensity", GfxMgr->mBloomDirtIntensity);

	glBindTextureUnit(0, mFb->GetTexID());

	glBindTextureUnit(1, mDirtMask->get()->GetID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mDirtMask->get()->GetID());

	for (uint8_t i = mMipmapCount - 1; i >= 1; --i)
	{
		mip_size.x = max(1.0, glm::floor(float(mSize.x) / glm::pow(2.0, i - 1)));
		mip_size.y = max(1.0, glm::floor(float(mSize.y) / glm::pow(2.0, i - 1)));

		upShader->SetUniform("uTexelSize", 1.0f / glm::vec2(mip_size));
		upShader->SetUniform("uMipLevel", i);

		glBindImageTexture(0, mFb->GetTexID(), i - 1, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mFb->GetTexID());

		glDispatchCompute(max(mip_size.x / 8, 1), max(mip_size.y / 8, 1), 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
	}

	Shader* shader = mShader->get();
	shader->Bind();

	shader->SetUniform("uTexture", 1);
	glActiveTexture(GL_TEXTURE1);
	mFb->BindTexture();

	GfxMgr->RenderModel(shader, "Quad");
}