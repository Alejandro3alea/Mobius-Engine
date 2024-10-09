#include "Rtti.h"

std::string Rtti::GetRttiName(const std::string & name)
{
	std::string tName(name);
	std::size_t c = tName.find("class ");
	while (c != std::string::npos)
	{
		auto s = tName.begin() + c;
		auto e = s + strlen("class ");
		tName.replace(s, e, "");
		c = tName.find("class ");
	}
	// remove struct
	c = tName.find("struct ");
	while (c != std::string::npos)
	{
		auto s = tName.begin() + c;
		auto e = s + strlen("struct ");
		tName.replace(s, e, "");
		c = tName.find("struct ");
	}
	return tName;
}
