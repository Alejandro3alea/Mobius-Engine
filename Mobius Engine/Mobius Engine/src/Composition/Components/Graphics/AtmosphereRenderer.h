#pragma once
#include "Renderable.h"
#include "Model/Model.h"
#include "SceneNode.h"

struct AtmosphereRenderer : public AlphaRenderable
{
	AtmosphereRenderer();
	virtual void OnGui() override;

	virtual void FromJson(json& Val) override;
	virtual json& ToJson(json& Val) override;

	virtual void Render(Shader* shader = nullptr) override;

protected:
	void SetUpNodes(const unsigned nodeIdx, SceneNode* parentNode);
	void ClearNodes();

public:
	std::vector<SceneNode*> mChildNodes;
	Resource<Model>* mSphere = nullptr;
};