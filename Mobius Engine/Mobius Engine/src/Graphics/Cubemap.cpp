#include "Cubemap.h"
#include "GfxMgr.h"
#include "Shader/Shader.h"
#include "Misc/ColorConsole.h"

#include <STB/stb_image.h>

#include <iostream>
#include <fstream>
#include <ext/matrix_transform.hpp>
#include <ext/matrix_clip_space.hpp>

Cubemap::Cubemap(const std::string& path)
{
	glGenTextures(1, &mID);
	Bind();

	std::ifstream input(path);
	if (!input.is_open() || !input.good() || input.eof())
	{
		PrintError("Error loading " + path);
		return;
	}

	for (int i = 0; i < 6; i++)
	{
		std::string currPath;
		std::getline(input, currPath);
		int w, h, colorChannels;
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load(currPath.c_str(), &w, &h, &colorChannels, 4);
		if(!data)
		{
			PrintError("Error loading cubemap texture");
			return;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		mSize = w;

		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void Cubemap::Bind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, mID);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

Skybox::Skybox(const std::string& path)
{
	glGenFramebuffers(1, &mFBOID);
	glGenRenderbuffers(1, &mRBOID);

	glBindFramebuffer(GL_FRAMEBUFFER, mFBOID);
	glBindRenderbuffer(GL_RENDERBUFFER, mRBOID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBOID);

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
	if (!data)
	{
		PrintError("Error loading skybox texture");
		return;
	}
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mID);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 2048, 2048, 0, GL_RGB, GL_FLOAT, nullptr);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Skybox rendering requirements
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);
	
	const glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	const std::array<glm::mat4, 6> views =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	ProcessSkyboxToCubemap(proj, views);
	ProcessIrradianceMap(proj, views);
	ProcessPrefilterMap(proj, views);

	// Reset
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void Skybox::Bind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, mID);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void Skybox::BindIrradianceMap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, mIrradianceMapID);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void Skybox::BindPrefilterMap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, mPrefilterMapID);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void Skybox::ProcessSkyboxToCubemap(const glm::mat4& proj, const std::array<glm::mat4, 6>& views)
{
	Shader* shader = GfxMgr->mDefaultShaders["SkyboxToCubemap"]->get();
	shader->Bind();
	shader->SetUniform("model", glm::mat4(1.0f));
	shader->SetUniform("proj", proj);
	shader->SetUniform("uHDR", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	glViewport(0, 0, 2048, 2048);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBOID);
	for (unsigned int i = 0; i < 6; ++i)
	{
		shader->SetUniform("view", views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GfxMgr->RenderModel(shader, "Cube");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Bind();
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void Skybox::ProcessIrradianceMap(const glm::mat4& proj, const std::array<glm::mat4, 6>& views)
{
	glGenTextures(1, &mIrradianceMapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mIrradianceMapID);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, mFBOID);
	glBindRenderbuffer(GL_RENDERBUFFER, mRBOID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128);

	// -----------------------------------------------------------------------------

	Shader* shader = GfxMgr->mDefaultShaders["CubemapIrradianceConvolution"]->get();
	shader->Bind();
	shader->SetUniform("model", glm::mat4(1.0f));
	shader->SetUniform("proj", proj);
	shader->SetUniform("uSkybox", 1);
	glActiveTexture(GL_TEXTURE1);
	Bind();

	glViewport(0, 0, 128, 128);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBOID);
	for (unsigned int i = 0; i < 6; ++i)
	{
		shader->SetUniform("view", views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mIrradianceMapID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GfxMgr->RenderModel(shader, "Cube");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void Skybox::ProcessPrefilterMap(const glm::mat4& proj, const std::array<glm::mat4, 6>& views)
{
	glGenTextures(1, &mPrefilterMapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mPrefilterMapID);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	Shader* shader = GfxMgr->mDefaultShaders["CubemapPrefilter"]->get();
	shader->Bind();
	shader->SetUniform("model", glm::mat4(1.0f));
	shader->SetUniform("proj", proj);
	shader->SetUniform("uSkybox", 1);
	glActiveTexture(GL_TEXTURE1);
	Bind();

	glBindFramebuffer(GL_FRAMEBUFFER, mFBOID);
	unsigned maxMipLevels = 5;
	for (unsigned mip = 0; mip < maxMipLevels; ++mip)
	{
		const unsigned mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		const unsigned mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		glBindRenderbuffer(GL_RENDERBUFFER, mRBOID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		shader->SetUniform("uRoughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			shader->SetUniform("view", views[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mPrefilterMapID, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			GfxMgr->RenderModel(shader, "Cube");
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
