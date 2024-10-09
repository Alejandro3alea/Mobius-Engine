#include "AudioListener.h"
#include "Audio/Sound.h"
#include "Composition/SceneNode.h"

void AudioListener::OnGui()
{
}

void AudioListener::Update(const glm::vec3& pos)
{
	glm::vec3 vel = pos - mPrevPos;
	alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	mPrevPos = pos;
}
