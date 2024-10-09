#include "GfxMgr.h"
#include "Shader/Shader.h"
#include "Cubemap.h"
#include "Model/Model.h"

#include "ColorConsole.h"
#include "Components/Graphics/Renderable.h"
#include "Components/Graphics/ParticleEmitter.h"
#include "Framebuffer.h"
#include "Resources/ResourceMgr.h"
#include "SceneNode.h"
#include "Camera.h"
#include "Editor.h"
#include "InputMgr.h"
#include "Timer.h"
#include "PostProcessing/BloomEffect.h"

#include <GL/glew.h>

#include <algorithm>
#include <Debug/GLDebug.h>

GraphicsManager* GraphicsManager::mpInstance;

void GraphicsManager::Initialize()
{
	// Initialize OpenGL library
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW Error: Failed to init" << std::endl;
		abort();
	}

	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	// enable depth buffering & back-face removal
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Avoid edges in cubemap reflections
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::Load()
{
	mDefaultShapes["Sphere"] = ResourceMgr->Load<Model>("data/engine/meshes/obj/Sphere.obj");
	mDefaultShapes["Cube"] = ResourceMgr->Load<Model>("data/engine/meshes/obj/Cube.obj");
	mDefaultShapes["Quad"] = ResourceMgr->Load<Model>("data/engine/meshes/obj/Quad.obj");

	mDefaultShaders["EditorGrid"] = ResourceMgr->Load<Shader>("data/engine/shaders/EditorGrid.shader");
	mDefaultShaders["Cubemap"] = ResourceMgr->Load<Shader>("data/engine/shaders/Cubemap.shader");
	mDefaultShaders["SkyboxToCubemap"] = ResourceMgr->Load<Shader>("data/engine/shaders/SkyboxToCubemap.shader");
	mDefaultShaders["CubemapIrradianceConvolution"] = ResourceMgr->Load<Shader>("data/engine/shaders/IrradianceConvolution.shader");
	mDefaultShaders["CubemapPrefilter"] = ResourceMgr->Load<Shader>("data/engine/shaders/CubemapPrefilter.shader");
	mDefaultShaders["BRDF_LUT"] = ResourceMgr->Load<Shader>("data/engine/shaders/BRDF_LUT.shader");

	mSkybox = ResourceMgr->Load<Skybox>("data/skybox/Field.hdr");

	mPostProcessEffects.push_back(std::unique_ptr<PostProcessEffect>(new BloomEffect("data/engine/shaders/SimpleScreen.shader")));
	mPostProcessEffects.push_back(std::unique_ptr<PostProcessEffect>(new PostProcessEffect("data/engine/shaders/PostProcess.shader")));

	for (unsigned i = 0; i < mPostProcessEffects.size(); i++)
		mPostProcessEffects[i]->Initialize();

	InitializeBRDFLUTTexture();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::Update()
{
	if (InputMgr->isButtonDown(InputMgr->eButtonRight) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		EditorCamera& cam = Editor->mEditorCam;
		if (InputMgr->isKeyDown(SDL_SCANCODE_A))
			cam.Move(-cam.right() * TimeMgr->deltaTime * 10.0f);
		if (InputMgr->isKeyDown(SDL_SCANCODE_D))
			cam.Move( cam.right() * TimeMgr->deltaTime * 10.0f);
		if (InputMgr->isKeyDown(SDL_SCANCODE_S))
			cam.Move(-cam.front() * TimeMgr->deltaTime * 10.0f);
		if (InputMgr->isKeyDown(SDL_SCANCODE_W))
			cam.Move( cam.front() * TimeMgr->deltaTime * 10.0f);
		if (InputMgr->isKeyDown(SDL_SCANCODE_Q))
			cam.Move(glm::vec3(0, -TimeMgr->deltaTime * 5.0f, 0));
		if (InputMgr->isKeyDown(SDL_SCANCODE_E))
			cam.Move(glm::vec3(0, TimeMgr->deltaTime * 5.0f, 0));

		glm::ivec2 mouseMovement = InputMgr->GetMouseMovement();
		cam.mAngle.x += glm::radians(static_cast<float>(mouseMovement.x)) * TimeMgr->deltaTime * 2.5f;
		cam.mAngle.y += glm::radians(static_cast<float>(mouseMovement.y)) * TimeMgr->deltaTime * 2.5f;
		float radius = glm::length(cam.mLookAt - cam.mPos);
		cam.mLookAt.x = cam.mPos.x + radius * sin(cam.mAngle.y) * cos(cam.mAngle.x);
		cam.mLookAt.y = cam.mPos.y + radius * cos(cam.mAngle.y);
		cam.mLookAt.z = cam.mPos.z + radius * sin(cam.mAngle.y) * sin(cam.mAngle.x);

		SDL_DisplayMode dm;
		SDL_GetDesktopDisplayMode(0, &dm);
		glm::ivec2 minLimits(-dm.w / 2, -dm.w / 2);
		glm::ivec2 maxLimits(dm.w / 2, dm.h / 2);

		glm::ivec2 mousePos = InputMgr->GetMousePos();
		glm::ivec2 finalMousePos = mousePos;
	}
	 
	if (InputMgr->isKeyPressed(SDL_SCANCODE_F1))
		ResourceMgr->ReloadShaders();

	// Update particle emitters
	for (ParticleEmitter* emitter : mParticleEmitters)
		emitter->Update();

	mTime += TimeMgr->deltaTime;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ComputeWorldSpace(SceneNode& pNode)
{
	Transform& wt = pNode.worldTransform;
	Transform& t = pNode.transform;
	wt.pos = t.pos;
	wt.scale = t.scale;
	wt.rotation = t.rotation;

	if (pNode.mParent)
		wt.Concatenate(pNode.mParent->worldTransform);

	for (unsigned int i = 0; i < pNode.mChildren.size(); i++)
		ComputeWorldSpace(*pNode.mChildren[i]);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::Render()
{
	ComputeWorldSpace(*Scene->mRoot.get());

	Editor->mEditorCam.UpdateVectors();

	if (!mPostProcessEffects.empty())
		mPostProcessEffects[0]->mFb->BindFramebuffer();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderScene(&Editor->mEditorCam);
	RenderSkybox(&Editor->mEditorCam);
	RenderAlphaComps(&Editor->mEditorCam);
	RenderGrid();
	RenderPostProcess();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::Shutdown()
{
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::RenderModel(Shader* shader, const std::string& modelName)
{
	if (mDefaultShapes[modelName] != nullptr)
		mDefaultShapes[modelName]->get()->Render(shader);
	else
		PrintError("Model named " + modelName + " does not exist.");
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::RenderScene(Camera* cam, Shader* shader)
{
	glm::mat4x4 view_cam = cam->GetViewMtx();
	glm::mat4x4 proj_cam = cam->GetProjMtx();
	glm::vec3 offset = cam->mPos;

	std::for_each(mRenderComps.begin(), mRenderComps.end(), [&](Renderable* comp) 
	{
		if (comp->mShader)
		{
			Shader* shader = comp->mShader->get();
			shader->Bind();
			shader->SetUniform("uLight", mLights);
			shader->SetUniform("uViewPos", offset);

			shader->SetUniform("uIrradianceMap", 6);
			glActiveTexture(GL_TEXTURE6);
			mSkybox->get()->BindIrradianceMap();

			shader->SetUniform("uPrefilterMap", 7);
			glActiveTexture(GL_TEXTURE7);
			mSkybox->get()->BindPrefilterMap();

			shader->SetUniform("uBRDFLUT", 8);
			glActiveTexture(GL_TEXTURE8);
			BindBRDFLUTTexture();

			shader->SetUniform("uTime", mTime);

			glm::mat4 model = comp->mOwner->worldTransform.GetModelMtx();
			shader->SetUniform("model", model);
			shader->SetUniform("view", view_cam);
			shader->SetUniform("proj", proj_cam);
			comp->Render();
		}
	});
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::RenderSkybox(Camera* cam)
{
	Shader* shader = mDefaultShaders["Cubemap"]->get();
	shader->Bind();

	// Matrices
	shader->SetUniform("model", glm::translate(glm::mat4(1.0f), glm::vec3(cam->mPos)));
	shader->SetUniform("view", cam->GetViewMtx());
	shader->SetUniform("proj", cam->GetProjMtx());

	// Skybox
	Skybox* skybox = mSkybox->get();
	shader->SetUniform("uSkybox", 0);
	glActiveTexture(GL_TEXTURE0);
	skybox->Bind();

	// Skybox rendering requirements
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	// For each frame just need to bind the VAO and call draw
	RenderModel(shader, "Cube");

	// Reset
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::RenderAlphaComps(Camera* cam)
{
	glm::mat4x4 view_cam = cam->GetViewMtx();
	glm::mat4x4 proj_cam = cam->GetProjMtx();
	glm::vec3 offset = cam->mPos;

	std::for_each(mAlphaRenderComps.begin(), mAlphaRenderComps.end(), [&](AlphaRenderable* comp)
	{
		if (comp->mShader)
		{
			Shader* shader = comp->mShader->get();
			shader->Bind();
			shader->SetUniform("view", view_cam);
			shader->SetUniform("proj", proj_cam);
			comp->Render();
		}
	});
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::RenderGrid()
{
	Shader* shader = mDefaultShaders["EditorGrid"]->get();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader->Bind();
	shader->SetUniform("view",  Editor->mEditorCam.GetViewMtx());
	shader->SetUniform("proj",  Editor->mEditorCam.GetProjMtx());
	shader->SetUniform("uFar",  Editor->mEditorCam.mFar);
	shader->SetUniform("uNear", Editor->mEditorCam.mNear);

	glm::vec3 gridPlane[6] = { { -1, -1, 0 }, { 1, -1, 0 }, { -1, 1, 0 }, { 1, -1, 0 }, { 1, 1, 0 }, { -1, 1, 0 } };

	GLuint mVAO, mVBO;
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Create Buffers
	glGenBuffers(1, &mVBO);

	// Bind a buffer of vertices
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(gridPlane[0]), &gridPlane[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gridPlane[0]), reinterpret_cast<void*>(0));

	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::RenderPostProcess()
{
	const size_t postProcessCount = mPostProcessEffects.size();
	for (unsigned i = 0; i < postProcessCount; i++)
	{
		// Last pass = render to main buffer
		unsigned fbIdx = (i + 1 < postProcessCount) ? mPostProcessEffects[i + 1]->mFb->GetFbID() : 0;
		glBindFramebuffer(GL_FRAMEBUFFER, fbIdx);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		PostProcessEffect* effect = mPostProcessEffects[i].get();
		Shader* shader = effect->mShader->get();
		effect->Render();
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::InitializeBRDFLUTTexture()
{
	unsigned int lutFBOID;
	unsigned int lutRBOID;
	glGenFramebuffers(1, &lutFBOID);
	glGenRenderbuffers(1, &lutRBOID);

	glBindFramebuffer(GL_FRAMEBUFFER, lutFBOID);
	glBindRenderbuffer(GL_RENDERBUFFER, lutRBOID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, lutRBOID);

	glGenTextures(1, &mBRDFLUTTexID);
	BindBRDFLUTTexture();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 1024, 1024, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, lutFBOID);
	glBindRenderbuffer(GL_RENDERBUFFER, lutRBOID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBRDFLUTTexID, 0);

	glViewport(0, 0, 1024, 1024);
	Shader* shader = GfxMgr->mDefaultShaders["BRDF_LUT"]->get();
	shader->Bind();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GfxMgr->RenderModel(shader, "Quad");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::OnWireframeModeChanged()
{
	if (mWireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void GraphicsManager::BindBRDFLUTTexture()
{
	glBindTexture(GL_TEXTURE_2D, mBRDFLUTTexID);
}
