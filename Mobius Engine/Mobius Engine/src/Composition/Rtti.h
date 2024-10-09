#pragma once
#include <typeinfo>
#include <string>

struct Rtti
{
	Rtti(const std::type_info& type) : mTypeName(GetRttiName(type.name())) {}

	template <typename T>
	Rtti(const T& type) : mTypeName(GetRttiName(typeid(type).name())) {}

	bool operator==(const Rtti& rhs) { return mTypeName == rhs.mTypeName; }
	bool operator!=(const Rtti& rhs) { return mTypeName != rhs.mTypeName; }

	const std::string & GetRttiType() const { return mTypeName; }

private:
	std::string GetRttiName(const std::string& name);

	std::string mTypeName;
};