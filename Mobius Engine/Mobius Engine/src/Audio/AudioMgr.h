#pragma once
#include "Misc/Singleton.h"
#include "AL/alc.h"
#include "AL/al.h"

#include <vector>

struct AudioEmitter;

class AudioManager
{
	Singleton(AudioManager);

	friend struct AudioEmitter;

public:
	void Initialize();
	void Shutdown();

	void Update();

private:
	ALCdevice* mDevice;
	ALCcontext* mContext;
	std::vector<AudioEmitter*> mAudioEmitters;
};

#define AudioMgr AudioManager::Instance()