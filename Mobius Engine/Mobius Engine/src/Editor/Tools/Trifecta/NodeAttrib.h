#pragma once
#include "ShaderNode.h"
#include <string>

enum AttribType
{
	ATTRIB_FLOAT,
	ATTRIB_VEC3,
	ATTRIB_VEC2,
	ATTRIB_INT
};

struct NodeAttribute
{
	NodeAttribute(const unsigned _id, const AttribType _type, const std::string& _name) : id(_id), type(_type), linkIdx(-1), name(_name) {}

	virtual ~NodeAttribute() {}

	unsigned id;
	AttribType type;

	int linkIdx;

	std::string name;

	void* parentNode;
};

template<typename T>
struct TNodeAttribute : public NodeAttribute
{
	TNodeAttribute(const unsigned _id, const AttribType _type, const std::string& _name, const T _val) : NodeAttribute(_id, _type, _name), val(_val) {}

	virtual ~TNodeAttribute() override {}

	T val;
};