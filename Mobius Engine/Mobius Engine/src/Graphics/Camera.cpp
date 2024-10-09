#include "Camera.h"
#include "WindowMgr.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
	mAspect = WindowMgr->mCurrentWindow->aspectRatio();
	mPos	= glm::vec3(0.f, 1.5f, 10.f);
	mLookAt = glm::vec3(0.f, 1.5f, -1.f);
	mUpVec	= glm::vec3(0.f, 1.f, 0.f);

	mAudioListener = new AudioListener();
}

glm::mat4x4 Camera::GetViewMtx()
{
	return glm::lookAt(mPos, mLookAt, mUpVec);
}

glm::mat4x4 Camera::GetProjMtx()
{
	return glm::perspective(glm::radians(mFOV), mAspect, mNear, mFar);
}

EditorCamera::EditorCamera() :	mAngle(glm::vec2(glm::radians(270.f), 
								glm::radians(90.f))), 
								mUp(0.f, 1.f, 0.f), 
								mRight(1.f, 0.f, 0.f), 
								mFront(0.f, 0.f, -1.f) {}

void EditorCamera::Move(const glm::vec3& vec)
{
	mPos	+= vec;
	mLookAt += vec;
}

void EditorCamera::UpdateVectors()
{
	mFront = glm::normalize(mLookAt - mPos);
	glm::vec3 tVec = (glm::length(mFront) < 0.0f) ? glm::vec3(1.f, 0.f, 0.f) : glm::vec3(-1.f, 0.f, 0.f);
	mRight = glm::cross(glm::vec3(0.f, -1.f, 0.f), mFront);
	mUp = glm::cross(mRight, mFront);

	mAudioListener->Update(mPos);
}
