#pragma once
#include "AudioComp.h"

struct AudioListener : public AudioComp
{
	virtual void OnGui() override;

	virtual void Update(const glm::vec3& pos) override;
};