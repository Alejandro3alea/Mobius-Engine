#include "PostProcessFx.h"
#include "Framebuffer.h"
#include "GfxMgr.h"

PostProcessEffect::PostProcessEffect(const std::string& shaderPath)
{
	mShader = ResourceMgr->Load<Shader>(shaderPath);
}

void PostProcessEffect::Initialize()
{
	mFb = std::shared_ptr<Framebuffer>(new Framebuffer(1, HDRBuffer()));
	mFb->Initialize();
}

void PostProcessEffect::Render()
{
	Shader* shader = mShader->get();
	shader->Bind();

	shader->SetUniform("uTexture", 1);
	glActiveTexture(GL_TEXTURE1);
	mFb->BindTexture();

	shader->SetUniform("uGamma", GfxMgr->mGamma);
	shader->SetUniform("uExposure", GfxMgr->mExposure);

	GfxMgr->RenderModel(shader, "Quad");
}
