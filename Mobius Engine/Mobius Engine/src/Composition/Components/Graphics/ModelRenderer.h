#pragma once
#include "Renderable.h"
#include "Model/Model.h"
#include "SceneNode.h"

struct ModelRenderer : public Renderable
{
	ModelRenderer(const std::string& modelPath = "data/engine/meshes/obj/Cube.obj");
	virtual void OnGui() override;

	virtual void FromJson(json& Val) override;
	virtual json& ToJson(json& Val) override;

	virtual void Render(Shader* shader = nullptr) override;

	Resource<Model>* SetModel(const std::string& path);
	Resource<Shader>* SetShader(const std::string& path);

protected:
	void SetUpNodes(const unsigned nodeIdx, SceneNode* parentNode);
	void ClearNodes();

public:
	std::vector<SceneNode*> mChildNodes;
	Resource<Model>* mModel = nullptr;
};

struct MeshRenderer : public Component
{
	MeshRenderer();
	void OnGui();

	/*void FromJson(json& val);
	void ToJson(json& val);*/

public:
	Mesh* mMesh = nullptr;
	ModelRenderer* mModelParent;
	size_t mNodeIdx;
};