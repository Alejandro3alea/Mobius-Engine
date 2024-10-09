#pragma once
#include "Component.h"
#include "ResourceMgr.h"
#include "Shader/Shader.h"

struct Renderable : public Component
{
	Renderable();
	~Renderable();

	virtual void Render(Shader* shader = nullptr) = 0;

	Resource<Shader>* mShader = nullptr;
};


struct AlphaRenderable : public Component
{
	AlphaRenderable();
	~AlphaRenderable();

	virtual void Render(Shader* shader = nullptr) = 0;

	Resource<Shader>* mShader = nullptr;
};
