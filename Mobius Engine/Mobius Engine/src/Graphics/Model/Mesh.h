#pragma once
#include "Primitive.h"

struct Shader;
struct Model;

struct Mesh
{
	void Render(Shader* shader) const;

public:
	std::vector<Primitive> mPrimitives;
	Model* mModel;
};