#pragma once
#include <glew.h>
#include <glm.hpp>

#include <string>
#include <array>

struct Cubemap
{
	Cubemap(const std::string& path);

	void Bind();

private:
	GLuint mID;
	GLuint mSize;
};

struct Skybox
{
	Skybox(const std::string& path);

	void Bind();
	void BindIrradianceMap();
	void BindPrefilterMap();

private:
	void ProcessSkyboxToCubemap(const glm::mat4& proj, const std::array<glm::mat4, 6>& view);
	void ProcessIrradianceMap(const glm::mat4& proj, const std::array<glm::mat4, 6>& view);
	void ProcessPrefilterMap(const glm::mat4& proj, const std::array<glm::mat4, 6>& view);

private:
	GLuint mID;
	GLuint mSize;

	GLuint mTextureID;
	GLuint mIrradianceMapID;
	GLuint mPrefilterMapID;

	GLuint mFBOID;
	GLuint mRBOID;
};