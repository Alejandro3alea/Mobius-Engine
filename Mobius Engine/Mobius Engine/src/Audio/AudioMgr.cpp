#include "AudioMgr.h"
#include "Sound.h"
#include "Components/Audio/AudioEmitter.h"

#include <iostream>

AudioManager* AudioManager::mpInstance;

void AudioManager::Initialize()
{
	mDevice = alcOpenDevice(nullptr);
	if (!mDevice)
	{
		std::cout << "AL Error: Failed to init default device." << std::endl;
		abort();
	}

	mContext = alcCreateContext(mDevice, nullptr);
	if (!mContext || !alcMakeContextCurrent(mContext))
	{
		std::cout << "AL Error: Failed to init current context." << std::endl;
		abort();
	}

	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(mDevice, "ALC_ENUMERATE_ALL_TEXT"))
		name = alcGetString(mDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(mDevice) != AL_NO_ERROR)
		name = alcGetString(mDevice, ALC_DEVICE_SPECIFIER);

	std::cout << "Opened \"" << name << "\"" << std::endl;
}

void AudioManager::Shutdown()
{
	alcDestroyContext(mContext);
	alcCloseDevice(mDevice);
}

void AudioManager::Update()
{
	for (auto it : mAudioEmitters)
		it->Update(it->mOwner->worldTransform.pos);
}