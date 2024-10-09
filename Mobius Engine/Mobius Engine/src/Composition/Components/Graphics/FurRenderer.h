#pragma once
#include "ModelRenderer.h"

struct FurRenderer : public ModelRenderer
{
	FurRenderer();
	virtual void OnGui() override;

	virtual void FromJson(json& Val) override;
	virtual json& ToJson(json& Val) override;

    void Update(const float dt);

	virtual void Render(Shader* shader = nullptr) override;

public:
	int mShellCount = 150;
    float mShellLength = 0.2f;
    float mDistAtt = 1.0f;
    float mDensity = 100.0f;
    float mNoiseMin = 0.0f;
    float mNoiseMax = 1.0f;
    float mThickness = 1.0f;
    float mCurvature = 1.0f;
    float mDisplacementStrength = 0.1f;
    float mOcclusionAtt = 1.0f;
    float mOcclusionBias = 0.0f;

    glm::vec3 mShellColor = glm::vec3(1.0f);
    glm::vec3 mSpringPos = glm::vec3(0.0f);

    Resource<Texture>* mTexture;

private:
    float mSpringMass = 2.0f;
    float mSpringConstant = 25.0f;
    float mSpringDrag = 0.998f;
    float mSpringVelocityFactor = 2.0f;

    glm::vec3 mLastPos = glm::vec3(0.0f);
    glm::vec3 mSpringVelocity = glm::vec3(0.0f);
};