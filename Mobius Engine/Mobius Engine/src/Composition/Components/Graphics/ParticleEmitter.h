#pragma once
#include "Renderable.h"
#include "Graphics/Texture.h"
#include <glm.hpp>

#include <list>

#define TIME_START 0
#define TIME_END 1
#define TIME_COUNT 2

#define RANDOM_MIN 0
#define RANDOM_MAX 1
#define RANDOM_COUNT 2

#define MAX_PARTICLES 10000

struct ParticleEmitter;
struct Model;

struct ParticleProperties
{
	float mLifetime = 1.0f;

	glm::vec3 mVelocity[TIME_COUNT];
	glm::vec3 mScale[TIME_COUNT];
	glm::vec4 mColor[TIME_COUNT];
	glm::vec4 mRotation[TIME_COUNT];
};

struct Particle
{
	Particle() = default;
	Particle(const glm::vec3& ownerPos, ParticleProperties prop[RANDOM_COUNT]);

	void Initialize(const glm::vec3& ownerPos, ParticleProperties prop[RANDOM_COUNT]);
	void Update(float dt);
	void Render(Shader* shader);

	bool IsAlive() { return mParticleTime > 0.0f; }

public:
	float mParticleTime = 0.0f;

	glm::vec3 mPos;
	glm::vec3 mScale;
	float mRot = 0.0f;

	ParticleProperties properties;

	ParticleEmitter* mEmitter;
};

struct ParticleEmitter : public AlphaRenderable
{
	ParticleEmitter();
	~ParticleEmitter();

	void Update();
	virtual void Render(Shader* shader = nullptr) override;

	void OnGui() override;

	//void FromJson(json& val) override;
	//void ToJson(json& val) override;

	void OnModelChanged(Model* newModel);

public:

	int mEmitRate = 500;
	unsigned mMaxParticles = MAX_PARTICLES;
	ParticleProperties properties[RANDOM_COUNT];

	bool mbEnablePhysics = true;

	float (*easeMethod_flt)(const float&, const float&, float);
	glm::vec3(*easeMethod_Vec3)(const glm::vec3&, const glm::vec3&, float);
	glm::vec4(*easeMethod_Vec4)(const glm::vec4&, const glm::vec4&, float);

	Resource<Texture>* mTex = nullptr;
	Model* mModel = nullptr;

	std::array<Particle, MAX_PARTICLES> mParticles;

private:
	float mCurrentTime = 0.0f;

	GLuint mFloatVBO; // All float/vec variables stored in a VBO

	std::array<glm::vec4, MAX_PARTICLES * 3> mFloatParams;


};