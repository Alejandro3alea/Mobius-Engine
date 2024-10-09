#pragma once
#include "Components/Audio/AudioListener.h"
#include <glm.hpp>

struct Camera
{
	Camera();

	glm::mat4x4 GetViewMtx();
	glm::mat4x4 GetProjMtx();

public:
	float mFOV = 45.f;
	float mAspect;
	float mNear = 0.1f, mFar = 600.f;
	glm::vec3 mPos, mLookAt, mUpVec;

	AudioListener* mAudioListener = nullptr;
};

struct EditorCamera : public Camera
{
	friend class GraphicsManager;

	EditorCamera();

	void Move(const glm::vec3& vec);

	void UpdateVectors();

public:
	glm::vec3 up()	  { return mUp;    }
	glm::vec3 front() { return mFront; }
	glm::vec3 right() { return mRight; }

	glm::vec2 mAngle;

private:
	glm::vec3 mUp, mFront, mRight; // Used in camera movement 
};