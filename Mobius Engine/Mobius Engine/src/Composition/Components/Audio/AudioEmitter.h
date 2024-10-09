#pragma once
#include "AudioComp.h"
#include "ResourceMgr.h"
#include "Audio/Sound.h"
#include "Audio/Sound.h"

struct AudioEmitter : public AudioComp
{
	AudioEmitter();
	~AudioEmitter();

	virtual void OnGui() override;

	/*void FromJson(json& val);
	void ToJson(json& val);*/

	virtual void Update(const glm::vec3& pos) override;

	void SetSound(const std::string& path);

public:
	void Play();

// Params
public:
	float mPitch	= 1.0f;
	float mGain		= 1.0f;
	bool mbLooping	= true;

protected:
	Resource<Sound>* mSound;

private:
	unsigned mSourceID;
};