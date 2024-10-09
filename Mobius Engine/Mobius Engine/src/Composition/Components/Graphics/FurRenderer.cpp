#include "FurRenderer.h"
#include "ImGuiWidgets.h"
#include "Editor.h"
#include "GfxMgr.h"
#include "Timer.h"

FurRenderer::FurRenderer()
{
	mShader = ResourceMgr->Load<Shader>("data/engine/Shaders/FurGravity.shader");

	SetModel("data/engine/meshes/obj/Suzanne.obj");

	mTexture = ResourceMgr->Load<Texture>("data/engine/textures/Rainbow.jpg");
}

void FurRenderer::OnGui()
{
	if (ImGui::BeginCompOnGui("Fur Renderer"), Editor->mIcons["FurRenderer"])
	{
		ImGui::OnGui("Shell count", mShellCount, 1, 300);
		ImGui::OnGui("Shell length", mShellLength, 0.01f, 0.0f);
		ImGui::OnGui("Shell color", mShellColor);
		ImGui::OnGui("Distance attenuation", mDistAtt, 0.01f, 0.01f, 3.0f);
		ImGui::OnGui("Density", mDensity, 0.1f, 1.0f, 1000.0f);
		ImGui::OnGui("Noise min", mNoiseMin, 0.01f, 0.0f, mNoiseMax);
		ImGui::OnGui("Noise max", mNoiseMax, 0.01f, mNoiseMin, 1.0f);
		ImGui::OnGui("Thickness", mThickness, 0.01f, 0.0f, 10.0f);
		ImGui::OnGui("Curvature", mCurvature, 0.01f, 0.0f, 10.0f);
		ImGui::OnGui("Displacement strength", mDisplacementStrength, 0.01f, 0.0f, 1.0f);
		ImGui::OnGui("Occlusion attribute", mOcclusionAtt, 0.01f, 0.0f, 5.0f);
		ImGui::OnGui("Occlusion bias", mOcclusionBias, 0.01f, 0.0f, 1.0f);

		ImGui::Separator();
		ImGui::OnGui("Spring mass", mSpringMass, 0.01f, 0.01f, 5.0f);
		ImGui::OnGui("Spring constant", mSpringConstant, 0.01f, 0.01f, 100.0f);
		ImGui::OnGui("Spring drag", mSpringDrag, 0.001f, 0.01f, 1.0f);
		ImGui::OnGui("Spring velocity factor", mSpringVelocityFactor, 0.01f, 0.01f, 10.0f);
	}
}

void FurRenderer::FromJson(json& Val)
{
}

json& FurRenderer::ToJson(json& Val)
{
	return Val;
}

void FurRenderer::Update(const float dt)
{
	mSpringVelocity *= mSpringDrag;

	const glm::vec3 CurrPos = mOwner->worldTransform.pos;
	mSpringVelocity += (mLastPos - CurrPos) * mSpringMass * mSpringVelocityFactor;
	mLastPos = CurrPos;

	const glm::vec3 gravityForce = glm::vec3(0.0f, -9.8f, 0.0f) * mSpringMass;
	const glm::vec3 springForce = -mSpringConstant * mSpringPos;

	const glm::vec3 acceleration = (gravityForce + springForce) * mSpringMass;

	mSpringVelocity += acceleration * dt;
	mSpringPos += mSpringVelocity * dt;
}

void FurRenderer::Render(Shader* shader)
{
	glDisable(GL_CULL_FACE);

	Update(TimeMgr->deltaTime);

	Shader* pShader = shader ? shader : mShader->get();
	pShader->Bind();
	pShader->SetUniform("uShellCount", mShellCount);
	pShader->SetUniform("uShellLength", mShellLength);
	pShader->SetUniform("uShellColor", mShellColor);
	pShader->SetUniform("uDistAtt", mDistAtt);
	pShader->SetUniform("uDensity", mDensity);
	pShader->SetUniform("uNoiseMin", mNoiseMin);
	pShader->SetUniform("uNoiseMax", mNoiseMax);
	pShader->SetUniform("uThickness", mThickness);
	pShader->SetUniform("uCurvature", mCurvature);
	pShader->SetUniform("uDisplacementStrength", mDisplacementStrength);
	pShader->SetUniform("uOcclusionAtt", mOcclusionAtt);
	pShader->SetUniform("uOcclusionBias", mOcclusionBias);
	pShader->SetUniform("uTime", GfxMgr->mTime);
	pShader->SetUniform("uSpringPos", mSpringPos);

	if (mTexture)
	{
		glActiveTexture(GL_TEXTURE1);
		mTexture->get()->Bind();
		pShader->SetUniform("uTexture", 1);
	}

	const int maxShells = 30;
	int renderedShells = 0;
	while (renderedShells < mShellCount)
	{
		pShader->SetUniform("uShellOffset", renderedShells);
		ModelRenderer::Render(shader);
		renderedShells += maxShells;
	}
}
