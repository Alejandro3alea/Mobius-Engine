#include "Serialization.h"
#include "Ensure.h"

void JsonToFile(const nlohmann::json& Val, const std::string& Filename)
{
    std::ofstream File(Filename);
    if (File.is_open())
    {
        File << Val.dump(4); // Use dump(4) for pretty printing with indentation of 4 spaces
        File.close();
    }
    else
    {
        // error print
    }
}

json FileToJson(const std::string Filename)
{
    std::ifstream File(Filename);

	Ensure(File.is_open() && File.good(), "Failed to open path for loading inputted Scene");

    // Read the content of the file
    std::string JsonString((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());

    // Convert file to json
    json Val = json::parse(JsonString);
	Val["Path"] << Filename;

    File.close();

    return Val;
}

#pragma region FROM_JSON
#pragma region DEFAULT_DATATYPES
json& operator>>(json& val, s8& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<s8>();
	return val;
}

json& operator>>(json& val, u8& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<u8>();
	return val;
}

json& operator>>(json& val, s16& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<s16>();
	return val;
}

json& operator>>(json& val, u16& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<u16>();
	return val;
}

json& operator>>(json& val, s32& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<s32>();
	return val;
}

json& operator>>(json& val, u32& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<u32>();
	return val;
}

json& operator>>(json& val, s64& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<s64>();
	return val;
}

json& operator>>(json& val, u64& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<u64>();
	return val;
}

json& operator>>(json& val, f32& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<f32>();
	return val;
}

json& operator>>(json& val, f64& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<f64>();
	return val;
}

json& operator>>(json& val, bool& type)
{
	Ensure(val != nullptr, "Json value is nullptr");
	type = val.get<bool>();
	return val;
}
#pragma endregion

json& operator>>(json& val, const char*& string)
{
	Ensure(val != nullptr, "Json value is nullptr");
	string = val.get<std::string>().c_str();
	return val;
}

json& operator>>(json& val, std::string& string)
{
	Ensure(val != nullptr, "Json value is nullptr");
	string = val.get<std::string>();
	return val;
}

json& operator>>(json& val, glm::vec2& vec)
{
	Ensure(val != nullptr, "Json value is nullptr");
	vec.x = val["x"].get<f32>();
	vec.y = val["y"].get<f32>();
	return val;
}

json& operator>>(json& val, glm::vec3& vec)
{
	Ensure(val != nullptr, "Json value is nullptr");
	vec.x = val["x"].get<f32>();
	vec.y = val["y"].get<f32>();
	vec.z = val["z"].get<f32>();
	return val;
}

json& operator>>(json& val, glm::vec4& col)
{
	Ensure(val != nullptr, "Json value is nullptr");
	col.r = val["r"].get<f32>();
	col.g = val["g"].get<f32>();
	col.b = val["b"].get<f32>();
	col.a = val["a"].get<f32>();
	return val;
}
json& operator>>(json& val, glm::quat& quat)
{
	Ensure(val != nullptr, "Json value is nullptr");
	quat.x = val["x"].get<f32>();
	quat.y = val["y"].get<f32>();
	quat.z = val["z"].get<f32>();
	quat.w = val["w"].get<f32>();
	return val;
}

json& operator>>(json& val, physx::PxVec3& vec)
{
	Ensure(val != nullptr, "Json value is nullptr");
	vec.x = val["x"].get<f32>();
	vec.y = val["y"].get<f32>();
	vec.z = val["z"].get<f32>();
	return val;
}

json& operator>>(json& val, physx::PxVec4& vec)
{
	Ensure(val != nullptr, "Json value is nullptr");
	vec.x = val["x"].get<f32>();
	vec.y = val["y"].get<f32>();
	vec.z = val["z"].get<f32>();
	vec.w = val["w"].get<f32>();
	return val;
}
json& operator>>(json& val, physx::PxQuat& quat)
{
	Ensure(val != nullptr, "Json value is nullptr");
	quat.x = val["x"].get<f32>();
	quat.y = val["y"].get<f32>();
	quat.z = val["z"].get<f32>();
	quat.w = val["w"].get<f32>();
	return val;
}
#pragma endregion

#pragma region TO_JSON

#pragma region DEFAULT_DATATYPES

json& operator<<(json& val, const s8 type) { return val = type; }
json& operator<<(json& val, const u8 type) { return val = type; }
json& operator<<(json& val, const s16 type) { return val = type; }
json& operator<<(json& val, const u16 type) { return val = type; }
json& operator<<(json& val, const s32 type) { return val = type; }
json& operator<<(json& val, const u32 type) { return val = type; }
json& operator<<(json& val, const s64 type) { return val = type; }
json& operator<<(json& val, const u64 type) { return val = type; }
json& operator<<(json& val, const f32 type) { return val = type; }
json& operator<<(json& val, const f64 type) { return val = type; }
json& operator<<(json& val, const bool type) { return val = type; }
#pragma endregion

json& operator<<(json& val, const char* string) { return val = string; }
json& operator<<(json& val, const std::string& string) { return val = string; }

json& operator<<(json& val, const glm::vec2& vec)
{
	json tVal;
	tVal["x"] = vec.x;
	tVal["y"] = vec.y;
	val = tVal;
	return val;
}

json& operator<<(json& val, const glm::vec3& vec)
{
	json tVal;
	tVal["x"] = vec.x;
	tVal["y"] = vec.y;
	tVal["z"] = vec.z;
	val = tVal;
	return val;
}

json& operator<<(json& val, const glm::vec4& col)
{
	json tVal;
	tVal["r"] = col.r;
	tVal["g"] = col.g;
	tVal["b"] = col.b;
	tVal["a"] = col.a;
	val = tVal;
	return val;
}

json& operator<<(json& val, glm::quat& quat)
{
	json tVal;
	tVal["x"] = quat.x;
	tVal["y"] = quat.y;
	tVal["z"] = quat.z;
	tVal["w"] = quat.w;
	val = tVal;
	return val;
}

json& operator<<(json& val, physx::PxVec3& vec)
{
	json tVal;
	tVal["x"] = vec.x;
	tVal["y"] = vec.y;
	tVal["z"] = vec.z;
	val = tVal;
	return val;
}

json& operator<<(json& val, physx::PxVec4& vec)
{
	json tVal;
	tVal["x"] = vec.x;
	tVal["y"] = vec.y;
	tVal["z"] = vec.z;
	tVal["w"] = vec.w;
	val = tVal;
	return val;
}

json& operator<<(json& val, physx::PxQuat& quat)
{
	json tVal;
	tVal["x"] = quat.x;
	tVal["y"] = quat.y;
	tVal["z"] = quat.z;
	tVal["w"] = quat.w;
	val = tVal;
	return val;
}
#pragma endregion