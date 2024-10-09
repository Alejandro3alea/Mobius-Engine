#pragma once
#include "ResourceMgr.h"
#include "Shader/Shader.h"

#include <memory>

struct Framebuffer;

struct PostProcessEffect
{
	PostProcessEffect(const std::string& shaderPath);

	virtual void Initialize();
	virtual void Render();

public:
	Resource<Shader>* mShader = nullptr;
	std::shared_ptr<Framebuffer> mFb;

	bool mbIsActivated = true;
};