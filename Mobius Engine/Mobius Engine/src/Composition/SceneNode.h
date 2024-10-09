#pragma once
#include "Transform.h"
#include "Component.h"

#include <vector>

struct SceneNode : public Serializable
{
	SceneNode(const std::string& name, SceneNode* parent = nullptr) : mName(name), mParent(parent) {}
	~SceneNode();

public:
	virtual void FromJson(json& val) override;
	virtual json& ToJson(json& val) override;

	void OnGui();

	template <typename T>
	T* AddComp(T* comp)
	{
		comp->mOwner = this;
		mComps.push_back(comp);
		comp->Initialize();

		return comp;
	}

public:
	std::string mName;

	SceneNode* mParent;

	// Components
	Transform transform;
	Transform worldTransform;
	std::vector<Component*> mComps;

	// Children nodes
	std::vector<std::shared_ptr<SceneNode>> mChildren;
};