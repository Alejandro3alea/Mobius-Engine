#pragma once
#include "Composition/SceneNode.h"
#include "Audio/Sound.h"

struct AudioComp : public Component
{
	virtual void Update(const glm::vec3& pos) = 0;

public:
	glm::vec3 mPrevPos = { 0.0f, 0.0f, 0.0f };
};