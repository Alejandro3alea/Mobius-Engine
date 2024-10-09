#pragma once
#include "Component.h"
#include "Light.h"

struct LightComp : public Component, Light
{
	LightComp();
	~LightComp();

	virtual void OnGui() override;

	virtual void FromJson(json& Val) override;
	virtual json& ToJson(json& Val) override;
};