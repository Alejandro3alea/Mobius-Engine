#include "AudioEmitter.h"
#include "Editor/ImGuiWidgets.h"
#include "Editor/Editor.h"
#include "Audio/AudioMgr.h"

AudioEmitter::AudioEmitter()
{
    alGenSources(1, &mSourceID);
    SetSound("data/engine/audio/sfx/bounce.wav");

    AudioMgr->mAudioEmitters.push_back(this);
}

AudioEmitter::~AudioEmitter()
{
    alDeleteSources(1, &mSourceID);
    AudioMgr->mAudioEmitters.erase(std::find(AudioMgr->mAudioEmitters.begin(), AudioMgr->mAudioEmitters.end(), this));
}

void AudioEmitter::OnGui()
{
	if (ImGui::BeginCompOnGui("Audio Emitter", Editor->mIcons["AudioEmitter"]))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);

		{
            ImGui::OnGui("Pitch", mPitch, 0.01f, 0.01f);
            ImGui::OnGui("Gain", mGain, 0.01f, 0.0f, 1.0f);
            ImGui::OnGui("Looping", mbLooping);

            if (ImGui::Button("Play"))
                Play();
		}

		ImGui::Columns(1);
		ImGui::PopStyleVar();
	}
}

void AudioEmitter::Update(const glm::vec3& pos)
{
    alSourcef(mSourceID, AL_PITCH, mPitch);
    alSourcef(mSourceID, AL_GAIN, mGain);
    alSourcei(mSourceID, AL_LOOPING, mbLooping ? AL_TRUE : AL_FALSE);

    glm::vec3 vel = pos - mPrevPos;
    alSource3f(mSourceID, AL_VELOCITY, vel.x, vel.y, vel.z);
    alSource3f(mSourceID, AL_POSITION, pos.x, pos.y, pos.z);
    mPrevPos = pos;
}

void AudioEmitter::SetSound(const std::string& path)
{
    mSound = ResourceMgr->Load<Sound>(path);
    alSourcei(mSourceID, AL_BUFFER, mSound->get()->GetBufferID());
}

void AudioEmitter::Play()
{
    alSourcePlay(mSourceID);
}
