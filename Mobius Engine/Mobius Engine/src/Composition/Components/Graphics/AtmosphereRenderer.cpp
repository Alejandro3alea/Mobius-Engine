#include "AtmosphereRenderer.h"

AtmosphereRenderer::AtmosphereRenderer()
{
	mShader = ResourceMgr->Load<Shader>("data/engine/Shaders/Atmosphere.shader");
	mSphere = ResourceMgr->Load<Model>("data/engine/meshes/obj/Sphere.obj");
}

void AtmosphereRenderer::OnGui()
{
}

void AtmosphereRenderer::FromJson(json& Val)
{
}

json& AtmosphereRenderer::ToJson(json& Val)
{
	return Val;
}

void AtmosphereRenderer::Render(Shader* shader)
{
	if (!mSphere) return;
	Model* pSphere = mSphere->get();
	Shader* pShader = shader ? shader : mShader->get();

	pSphere->Render(pShader);

	// Set everything back to defaults once configured for good practice
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(0);
}

void AtmosphereRenderer::SetUpNodes(const unsigned nodeIdx, SceneNode* parentNode)
{
}

void AtmosphereRenderer::ClearNodes()
{
}
