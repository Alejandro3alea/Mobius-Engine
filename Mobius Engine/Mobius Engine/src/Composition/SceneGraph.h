#pragma once
#include "Singleton.h"
#include "SceneNode.h"

#include <memory>

struct SceneGraph : public Serializable
{
	Singleton(SceneGraph);

	void NewScene();
	void Clear();

	void OnGui();

	json CreateNewJson(std::string& filePath);

	virtual void FromJson(json& val) override;
	virtual json& ToJson(json& val) override;
	json GetJson() { json val; ToJson(val); return val; }

	// Object Management
	SceneNode& CreateNode();

	std::string GetName() { return mName; }
	void SetName(const std::string& Name) { mName = Name; }

private:
	void RecursiveNodeOnGui(SceneNode* node);

public:
	std::string mName = "temp";
	std::string mPath = "temp.json";
	std::unique_ptr<SceneNode> mRoot;

private:
	unsigned nodeSpawnIdx = 0;
};

#define Scene SceneGraph::Instance()