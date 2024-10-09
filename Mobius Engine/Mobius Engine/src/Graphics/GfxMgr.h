#pragma once
#include "Misc/Singleton.h"
#include "ResourceMgr.h"
#include "PostProcessing/PostProcessFx.h"

#include <vector>
#include <string>
#include <map>

struct Renderable;
struct AlphaRenderable;
struct Cubemap;
struct Skybox;
struct Camera;
struct Shader;
struct Model;
struct LightComp;
struct ParticleEmitter;

class GraphicsManager
{
	Singleton(GraphicsManager);

	void Initialize();
	void Load();
	void Update();
	void Render();
	void Shutdown();

	void RenderModel(Shader* shader, const std::string& modelName = "Quad");

	void OnWireframeModeChanged();

	void BindBRDFLUTTexture();

	Resource<Skybox>* GetSkybox() { return mSkybox; }
	void SetSkybox(const std::string& path)
	{
		mSkybox = ResourceMgr->Load<Skybox>(path);
	}

private:
	void RenderScene(Camera* cam, Shader* shader = nullptr);
	void RenderSkybox(Camera* cam);
	void RenderAlphaComps(Camera* cam);
	void RenderGrid();
	void RenderPostProcess();

	void InitializeBRDFLUTTexture();

public:
	std::vector<LightComp*> mLights;
	std::vector<Renderable*> mRenderComps;
	std::vector<AlphaRenderable*> mAlphaRenderComps;
	std::vector<ParticleEmitter*> mParticleEmitters;
	std::vector<std::unique_ptr<PostProcessEffect>> mPostProcessEffects;
	bool mWireframeMode = false;

	// @TODO: Should be private.
	std::map<std::string, Resource<Shader>*> mDefaultShaders;
	std::map<std::string, Resource<Model>*> mDefaultShapes;
	float mTime = 0.0f;

	// Bloom
	bool  mBloomEnabled = true;
	float mBloomThreshold = 1.5f;
	float mBloomKnee = 0.1f;
	float mBloomIntensity = 1.0f;
	float mBloomDirtIntensity = 1.0f;

	// Gamma & Exposure
	bool  mGammaAttEnabled = true;
	float mGamma = 1.0f;
	float mExposure = 1.0f;

private:
	Resource<Skybox>* mSkybox;

	GLuint mBRDFLUTTexID;
};

#define GfxMgr GraphicsManager::Instance()