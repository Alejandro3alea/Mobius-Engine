#pragma once
#include "GL/glew.h"

#include <vector>

struct Primitive
{
	int mMatIdx = 0;
	int mCount = 0;
	int mIdxOffset = 0;

	GLenum mRenderMode = GL_TRIANGLES;
	GLenum mIdxType = GL_UNSIGNED_INT;

	GLuint mVAO, mEBO;

	bool mUsingIndices = true;
};