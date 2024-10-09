#pragma once
#include "WindowMgr.h"
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

struct Buffer
{
	enum class BufferType {
		ColorBuffer,
		HDRBuffer,
		DepthBuffer,
		StencilBuffer,
		BloomBuffer
	};

	Buffer(const BufferType bufType, const unsigned width, const unsigned height) : mBufferType(bufType), mSize(width, height) {}

	virtual GLuint Construct(const size_t bufferIdx) = 0;

public:
	const BufferType mBufferType = BufferType::HDRBuffer;
	const glm::uvec2 mSize = {};
};

struct ColorBuffer : public Buffer
{
	ColorBuffer(const glm::uvec2 size = WindowMgr->mCurrentWindow->size()) : Buffer(BufferType::ColorBuffer, size.x, size.y) {}
	ColorBuffer(const unsigned width, const unsigned height) : Buffer(BufferType::ColorBuffer, width, height) {}

	virtual GLuint Construct(const size_t bufferIdx) override;
};

struct HDRBuffer : public Buffer
{
	HDRBuffer(const glm::uvec2 size = WindowMgr->mCurrentWindow->size()) : Buffer(BufferType::HDRBuffer, size.x, size.y) {}
	HDRBuffer(const unsigned width, const unsigned height) : Buffer(BufferType::HDRBuffer, width, height) {}

	virtual GLuint Construct(const size_t bufferIdx) override;
};

struct DepthBuffer : public Buffer
{
	DepthBuffer(const glm::uvec2 size = WindowMgr->mCurrentWindow->size()) : Buffer(BufferType::DepthBuffer, size.x, size.y) {}
	DepthBuffer(const unsigned width, const unsigned height) : Buffer(BufferType::DepthBuffer, width, height) {}

	virtual GLuint Construct(const size_t bufferIdx) override;
};

struct StencilBuffer : public Buffer
{
	StencilBuffer(const glm::uvec2 size = WindowMgr->mCurrentWindow->size()) : Buffer(BufferType::StencilBuffer, size.x, size.y) {}
	StencilBuffer(const unsigned width, const unsigned height) : Buffer(BufferType::StencilBuffer, width, height) {}

	virtual GLuint Construct(const size_t bufferIdx) override;
};

struct BloomBuffer : public Buffer
{
	BloomBuffer(unsigned mipmapCount = 1, const glm::uvec2 size = WindowMgr->mCurrentWindow->size()) : Buffer(BufferType::BloomBuffer, size.x, size.y), mMipmapCount(mipmapCount) {}
	BloomBuffer(unsigned mipmapCount, const unsigned width, const unsigned height) : Buffer(BufferType::HDRBuffer, width, height), mMipmapCount(mipmapCount) {}

	virtual GLuint Construct(const size_t bufferIdx) override;

private:
	unsigned mMipmapCount;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Framebuffer
{
	friend class GraphicsManager;
	friend struct ShaderProgram;

	template <class T, class... Ts>
	Framebuffer(const unsigned int count, T fb, Ts... rest);

	void Initialize(const bool useDepthRBO = true);

	void BindFramebuffer();
	void UnbindFramebuffer();

	void BindTexture(const unsigned idx = 0u);
	void UnbindTexture();

	glm::uvec2 BufferSize(const unsigned idx = 0u) const { return mSize[idx]; }
	size_t size() const noexcept { return mBufferCount; }

	unsigned GetTexID(const unsigned idx = 0) const { return mTexID[0]; }
	unsigned GetFbID() const noexcept { return mFbID; }

private:
	Framebuffer(const unsigned int count) : mBufferCount(0) {}


public:
	bool mUseDepthBuffer = false; // Redundant for depth buffers

private:
	GLuint mFbID = 0;
	bool mbFbCreated = false;

	size_t mBufferCount;
	std::vector<Buffer::BufferType> mBuffers;
	std::vector<unsigned> mTexID;
	std::vector<glm::uvec2> mSize;
};

template<class T, class ...Ts>
inline Framebuffer::Framebuffer(const unsigned int count, T fb, Ts ...rest) : Framebuffer(count, rest...)
{
	if (!mbFbCreated)
	{
		glGenFramebuffers(1, &mFbID);
		mbFbCreated = true;

		mBuffers.resize(static_cast<size_t>(count));
		mTexID.resize(static_cast<size_t>(count));
		mSize.resize(static_cast<size_t>(count));
	}

	mBufferCount++;
	BindFramebuffer();

	size_t currIdx = count - mBufferCount;
	mBuffers[currIdx] = fb.mBufferType;
	mSize[currIdx] = fb.mSize;
	mTexID[currIdx] = fb.Construct(currIdx);

	UnbindFramebuffer();
}
