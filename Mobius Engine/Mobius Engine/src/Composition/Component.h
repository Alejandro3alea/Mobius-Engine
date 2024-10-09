#pragma once
#include "Misc/Serialization.h"

struct SceneNode;

struct Component : public Serializable
{
	Component(SceneNode* Owner = nullptr) : mOwner(Owner) {}

	virtual void Initialize() {}

	virtual void FromJson(json& val) {}
	virtual json& ToJson(json& val) { return val; }

	virtual void OnGui() {}

public:
	SceneNode* mOwner = nullptr;
};

inline void operator>>(json& lhs, Component* rhs) { rhs->FromJson(lhs); }
inline json& operator<<(json& lhs, Component* rhs) { return rhs->ToJson(lhs); }
