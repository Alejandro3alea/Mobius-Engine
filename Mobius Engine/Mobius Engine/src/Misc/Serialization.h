#pragma once
#include "DataTypes.h"

#include <Json/json.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <PhysX/PxPhysics.h>

#include <fstream>

using namespace nlohmann;

void JsonToFile(const nlohmann::json& Val, const std::string& Filename);
json FileToJson(const std::string Filename);

struct Serializable
{
	virtual void FromJson(json& val) {}
	virtual json& ToJson(json& val) { return val; }
};

#pragma region FROM_JSON
json& operator>>(json& val, s8& type);
json& operator>>(json& val, u8& type);
json& operator>>(json& val, s16& type);
json& operator>>(json& val, u16& type);
json& operator>>(json& val, s32& type);
json& operator>>(json& val, u32& type);
json& operator>>(json& val, s64& type);
json& operator>>(json& val, u64& type);
json& operator>>(json& val, f32& type);
json& operator>>(json& val, f64& type);
json& operator>>(json& val, bool& type);

json& operator>>(json& val, const char*& string);
json& operator>>(json& val, std::string& string);

json& operator>>(json& val, glm::vec2& vec);
json& operator>>(json& val, glm::vec3& vec);
json& operator>>(json& val, glm::vec4& col);
json& operator>>(json& val, glm::quat& col);
json& operator>>(json& val, physx::PxVec3& vec);
json& operator>>(json& val, physx::PxVec4& vec);
json& operator>>(json& val, physx::PxQuat& vec);
#pragma endregion

#pragma region TO_JSON
json& operator<<(json& val, const s8  type);
json& operator<<(json& val, const u8  type);
json& operator<<(json& val, const s16 type);
json& operator<<(json& val, const u16 type);
json& operator<<(json& val, const s32 type);
json& operator<<(json& val, const u32 type);
json& operator<<(json& val, const s64 type);
json& operator<<(json& val, const u64 type);
json& operator<<(json& val, const f32 type);
json& operator<<(json& val, const f64 type);
json& operator<<(json& val, const bool type);

json& operator<<(json& val, const char* string);
json& operator<<(json& val, const std::string& string);

json& operator<<(json& val, const glm::vec2& vec);
json& operator<<(json& val, const glm::vec3& vec);
json& operator<<(json& val, const glm::vec4& col);
json& operator<<(json& val, glm::quat& col);
json& operator<<(json& val, physx::PxVec3& vec);
json& operator<<(json& val, physx::PxVec4& vec);
json& operator<<(json& val, physx::PxQuat& vec);
#pragma endregion