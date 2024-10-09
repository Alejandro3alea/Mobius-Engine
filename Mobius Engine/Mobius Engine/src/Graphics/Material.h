#pragma once
#include "ResourceMgr.h"
#include "Texture.h"

struct Material
{
	glm::vec3 mColor = { 1.0f, 1.0f, 1.0f };
	float mAmbient = 0.2f;
	float mMetallic = 0.9f;
	float mRoughness = 0.1f;

	Resource<Texture>* mAlbedoTex	 = nullptr;
	Resource<Texture>* mNormalTex	 = nullptr;
	Resource<Texture>* mMetallicTex  = nullptr;
	Resource<Texture>* mRoughnessTex = nullptr;
	Resource<Texture>* mOclussionTex = nullptr;
	Resource<Texture>* mEmissiveTex  = nullptr;
};