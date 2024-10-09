#include "ParticleEmitter.h"
#include "Graphics/GfxMgr.h"
#include "Model/Model.h"
#include "Misc/RNG.h"
#include "Misc/Timer.h"
#include "Math/Easing.h"
#include "SceneNode.h"
#include "Editor/ImGuiWidgets.h"
#include "Editor/Editor.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

Particle::Particle(const glm::vec3& ownerPos, ParticleProperties prop[RANDOM_COUNT])
{
    Initialize(ownerPos, prop);
}

void Particle::Initialize(const glm::vec3& ownerPos, ParticleProperties prop[RANDOM_COUNT])
{
    properties.mVelocity[TIME_START] = RNG->GetVec3(prop[RANDOM_MIN].mVelocity[TIME_START], prop[RANDOM_MAX].mVelocity[TIME_START]);
    properties.mVelocity[TIME_END] = RNG->GetVec3(prop[RANDOM_MIN].mVelocity[TIME_END], prop[RANDOM_MAX].mVelocity[TIME_END]);

    properties.mScale[TIME_START] = RNG->GetVec3(prop[RANDOM_MIN].mScale[TIME_START], prop[RANDOM_MAX].mScale[TIME_START]);
    properties.mScale[TIME_END] = RNG->GetVec3(prop[RANDOM_MIN].mScale[TIME_END], prop[RANDOM_MAX].mScale[TIME_END]);

    properties.mColor[TIME_START] = RNG->GetVec4(prop[RANDOM_MIN].mColor[TIME_START], prop[RANDOM_MAX].mColor[TIME_START]);
    properties.mColor[TIME_END] = RNG->GetVec4(prop[RANDOM_MIN].mColor[TIME_END], prop[RANDOM_MAX].mColor[TIME_END]);

    mPos = ownerPos;
    mScale = properties.mScale[TIME_START];
    mParticleTime = properties.mLifetime;
}

void Particle::Update(float dt)
{
    if (mParticleTime < 0.0f)
        return;

	float normalizedTime = 1.0f - mParticleTime / properties.mLifetime;

	if (mEmitter->mbEnablePhysics)
	{
		mPos += mEmitter->easeMethod_Vec3(properties.mVelocity[TIME_START], properties.mVelocity[TIME_END], normalizedTime);
	}

	// @TODO: finish particles update

	mParticleTime -= dt;
}

void Particle::Render(Shader* shader)
{
	//shader->SetUniform("model", transform.getModelMtx());
}

ParticleEmitter::ParticleEmitter()
{
	GfxMgr->mParticleEmitters.push_back(this);

	mShader = ResourceMgr->Load<Shader>("data/engine/shaders/Particle.shader");
	mTex = ResourceMgr->Load<Texture>("data/engine/textures/White.jpg");

	mModel = GfxMgr->mDefaultShapes["Quad"]->get();

	easeMethod_flt = Lerp;
	easeMethod_Vec3 = Lerp;
	easeMethod_Vec4 = Lerp;

    for (int i = 0; i < mParticles.size(); i++)
        mParticles[i].mEmitter = this;
}

ParticleEmitter::~ParticleEmitter()
{
	GfxMgr->mParticleEmitters.erase(std::find(GfxMgr->mParticleEmitters.begin(), GfxMgr->mParticleEmitters.end(), this));
}

void ParticleEmitter::Update()
{
	int emitCount;

	switch (mEmitRate)
	{
	case -1:
		emitCount = mMaxParticles - mParticles.size();
		break;
	default:
		emitCount = min(mEmitRate, static_cast<int>(mMaxParticles - (mParticles.size() + mEmitRate)));
	}

    int idx = mParticles.size() - 1;
	for (int i = 0; i < emitCount && idx > 0; i++, idx--)
	{
        if (mParticles[idx].IsAlive())
            break;

		mParticles[idx].Initialize(mOwner->transform.pos, properties);
	}

	std::for_each(mParticles.begin(), mParticles.end(), [](Particle& particle) {
        particle.Update(TimeMgr->deltaTime);
		});

    //std::sort(mParticles.begin(), mParticles.end());
}

void ParticleEmitter::Render(Shader* shader)
{
	if (!mShader)
		return;

	Shader* pShader = shader != nullptr ? shader : mShader->get();
	pShader->Bind();

	glActiveTexture(GL_TEXTURE0);
	mTex->get()->Bind();
	pShader->SetUniform("uTextureData", 0);

	std::for_each(mParticles.begin(), mParticles.end(), [this, pShader](Particle& particle) {
		particle.Render(pShader);
		mModel->Render(pShader);
		});
}

void ParticleEmitter::OnGui()
{
	/*if (BeginCompOnGui("Particle Emitter"))
	{
		SetProperty("Emit Rate", mEmitRate);
		SetProperty("Max Particles", mMaxParticles);

		SetProperty("Velocity: start MIN", properties[RANDOM_MIN].mVelocity[TIME_START]);

		SetProperty("Velocity: start MIN", properties[RANDOM_MIN].mVelocity[TIME_START]);
		SetProperty("Velocity: start MAX", properties[RANDOM_MAX].mVelocity[TIME_START]);
		SetProperty("Velocity: end MIN", properties[RANDOM_MIN].mVelocity[TIME_END]);
		SetProperty("Velocity: end MAX", properties[RANDOM_MAX].mVelocity[TIME_END]);

		ImGui::TreePop();
	}*/

    if (ImGui::BeginCompOnGui("Particle Emitter"), Editor->mIcons["Particles"])
    {
        ImGui::OnGui("Emit Rate", mEmitRate);
        ImGui::OnGui("Max Particles", mMaxParticles);

        ImGui::OnGui("Velocity: start MIN", properties[RANDOM_MIN].mVelocity[TIME_START]);

        ImGui::OnGui("Velocity: start MIN", properties[RANDOM_MIN].mVelocity[TIME_START]);
        ImGui::OnGui("Velocity: start MAX", properties[RANDOM_MAX].mVelocity[TIME_START]);
        ImGui::OnGui("Velocity: end MIN", properties[RANDOM_MIN].mVelocity[TIME_END]);
        ImGui::OnGui("Velocity: end MAX", properties[RANDOM_MAX].mVelocity[TIME_END]);
    }
}

void ParticleEmitter::OnModelChanged(Model* newModel)
{
    /*
    layout(location = 0) in vec3 vPos;
    layout(location = 1) in vec2 vTexCoord;
    layout(location = 2) in vec3 vNormal;
    layout(location = 3) in vec3 vTangent;
    layout(location = 4) in vec3 vBitangent;
    */

    std::vector<GLuint> VAOlist = newModel->GetVAOs();

    for (const GLuint currVAO : VAOlist)
    {
        glBindVertexArray(currVAO);

        glGenBuffers(1, &mFloatVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mFloatVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * sizeof(glm::vec4), &mFloatParams[0], GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec2), (void*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec2), (void*)(sizeof(glm::vec2)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec2), (void*)(2 * sizeof(glm::vec2)));

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
    }

    glBindVertexArray(0);
}

/*
void ParticleEmitter::FromJson(json& val)
{
	json tVal = val["Particle Emitter"];
	tVal[".Emit Rate"] >> mEmitRate;
	tVal[".Max Particles"] >> mMaxParticles;

	json propertiesVal = tVal["Properties"];
	json velocityVal = tVal["Velocity"];
	json velStartVal = tVal["Start"];
	velStartVal["Min"] >> properties[RANDOM_MIN].mVelocity[TIME_START];
	velStartVal["Max"] >> properties[RANDOM_MAX].mVelocity[TIME_START];
	json velEndVal = tVal["End"];
	velEndVal["Min"] >> properties[RANDOM_MIN].mVelocity[TIME_END];
	velEndVal["Max"] >> properties[RANDOM_MAX].mVelocity[TIME_END];
}

void ParticleEmitter::ToJson(json& val)
{
	json tVal;
	tVal[".Emit Rate"] << mEmitRate;
	tVal[".Max Particles"] << mMaxParticles;

	json propertiesVal;
	json velocityVal, velStartVal, velEndVal;
	velStartVal["Min"] << properties[RANDOM_MIN].mVelocity[TIME_START];
	velStartVal["Max"] << properties[RANDOM_MAX].mVelocity[TIME_START];
	propertiesVal["Start"] = velStartVal;
	velEndVal["Min"] << properties[RANDOM_MIN].mVelocity[TIME_END];
	velEndVal["Max"] << properties[RANDOM_MAX].mVelocity[TIME_END];
	propertiesVal["End"] = velEndVal;
	propertiesVal["Velocity"] = velocityVal;

	tVal["Properties"] = propertiesVal;
	val["Particle Emitter"] = tVal;
}
*/


int getActiveVBOCount(GLuint vao)
{
    GLint maxVertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

    int activeVBOs = 0;

    for (int i = 0; i < maxVertexAttribs; ++i)
    {
        GLint bufferBinding;
        glBindVertexArray(vao);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &bufferBinding);
        glBindVertexArray(0); // Unbind the VAO

        if (bufferBinding != 0)
            activeVBOs++;
    }

    return activeVBOs;
}

// Function to check if a vertex attribute is enabled
bool isAttributeEnabled(GLuint vao, GLuint attributeIndex) {
    GLint isEnabled;
    glBindVertexArray(vao);
    glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &isEnabled);
    glBindVertexArray(0);  // Unbind the VAO

    return (isEnabled == GL_TRUE);
}

/*
NoteRenderer::NoteRenderer(Chart* inChart) : mChart(inChart)
{
    SetShader("data/engine/shaders/NoteRenderer.shader");
    mTexture = ResourceMgr->LoadFromBasePath<Texture>("data/noteskins/USWCelSM5/_Down Tap Note 16x8.png");

    UpdateParams();

    mVAO = GfxMgr->CreateQuadModel();

    mXPositions = { -3.0f, -1.0f, 1.0f, 3.0f };

    mRotations.push_back(glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    mRotations.push_back(glm::mat4(1.0f));
    mRotations.push_back(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    mRotations.push_back(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void NoteRenderer::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uZoom", Editor->mZoom);

    glActiveTexture(GL_TEXTURE1);
    mTexture->get()->Bind();
    currShader->SetUniform("uTexture", 1);

    currShader->SetUniform("uXPositions", mXPositions);
    currShader->SetUniform("uRotations", mRotations);

    glBindVertexArray(mVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, MAX_NOTES);

    int vboCount = getActiveVBOCount(mVAO);
    bool check0 = isAttributeEnabled(mVAO, 0);
    bool check1 = isAttributeEnabled(mVAO, 1);
    bool check2 = isAttributeEnabled(mVAO, 2);
    bool check3 = isAttributeEnabled(mVAO, 3);
    bool check4 = isAttributeEnabled(mVAO, 4);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void NoteRenderer::UpdateParams()
{
    // @TODO: Note styles
    const std::pair<float, unsigned> coords[] = {
        { 0.0f,			 448 },
        { 1.0f / 2.0f,	 384 },
        { 1.0f / 3.0f,	 320 },
        { 2.0f / 3.0f,	 320 },
        { 1.0f / 4.0f,	 256 },
        { 3.0f / 4.0f,	 256 },
        { 1.0f / 6.0f,	 192 },
        { 5.0f / 6.0f,	 192 },
        { 1.0f / 8.0f,	 128 },
        { 3.0f / 8.0f,	 128 },
        { 5.0f / 8.0f,	 128 },
        { 7.0f / 8.0f,	 128 },
        { 1.0f / 12.0f,	 64 },
        { 5.0f / 12.0f,	 64 },
        { 7.0f / 12.0f,	 64 },
        { 11.0f / 12.0f, 64 },
        { 1.0f / 5.0f,	 0 }
    };

    const size_t noteCount = mChart->mNotes.size();
    auto it = mChart->mNotes.begin();
    for (unsigned i = 0; i < MAX_NOTES; i++)
    {
        if (i >= noteCount)
        {
            for (; i < MAX_NOTES; i++)
            {
                unsigned floatIdx = i * 3;
                mFloatParams[floatIdx].x = -1.0f;
            }

            return;
        }

        unsigned floatIdx = i * 3;
        Note* note = *it;

        if (dynamic_cast<MineNote*>(note) != nullptr)
        {
            mFloatParams[floatIdx].x = -1.0f;
            it++;
            continue;
        }


        const float pos = note->mPos;
        const float decimalPart = pos - static_cast<int>(pos);
        unsigned currbeatUVOffset;
        for (unsigned idx = 0; idx < 8; idx++)
        {
            currbeatUVOffset = coords[idx].second;
            if (coords[idx].first == decimalPart)
                break;
        }

        mFloatParams[floatIdx] = glm::vec2(note->mDir, pos);
        mFloatParams[floatIdx + 1] = SetTextureScale(64, 64);
        mFloatParams[floatIdx + 2] = SetTextureOffset(0, currbeatUVOffset);

        it++;
    }
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void NoteRenderer::UpdateVBOs()
{
    UpdateParams();

    glBindBuffer(GL_ARRAY_BUFFER, mFloatVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_NOTES * 3 * sizeof(glm::vec2), &mFloatParams[0]);
}*/