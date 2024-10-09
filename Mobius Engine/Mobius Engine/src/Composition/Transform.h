#pragma once
#include "Misc/Serialization.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform : public Serializable
{
	Transform() : pos(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), rotation(glm::identity<glm::quat>()) {}
	Transform(const glm::vec3& _pos, const glm::vec3& _scale, const glm::quat& _rotation) : pos(_pos), scale(_scale), rotation(_rotation) {}

	glm::vec3 pos;
	glm::vec3 scale;
	glm::quat rotation;

public:
	glm::mat4 GetModelMtx();

	Transform& Concatenate(const Transform& rhs);
	Transform& InvConcatenate(const Transform& rhs);

	const Transform operator+(const Transform& rhs);
	const Transform operator-(const Transform& rhs);

	void OnGui();

	virtual void FromJson(json& Val) override;
	virtual json& ToJson(json& Val) override;
};