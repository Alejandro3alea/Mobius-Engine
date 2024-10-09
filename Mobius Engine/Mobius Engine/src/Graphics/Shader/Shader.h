#pragma once
#include "GL/glew.h"
#include "glm.hpp"

#include <vector>
#include <string>

struct LightComp;
struct Material;

// @TODO: TEMP
struct TrifectaLight;

struct ShaderBase
{
	void Bind() { glUseProgram(mID); }
	void Unbind() { glUseProgram(0); }

	GLuint GetID() const { return mID; }

#pragma region SetUniform
	void SetUniform(const std::string& name, const bool val) const noexcept;
	void SetUniform(const std::string& name, const int val) const noexcept;
	void SetUniform(const std::string& name, const GLuint val) const noexcept;
	void SetUniform(const std::string& name, const size_t val) const noexcept;
	void SetUniform(const std::string& name, const float val) const noexcept;
	void SetUniform(const std::string& name, const glm::vec2& vec) const noexcept;
	void SetUniform(const std::string& name, const glm::vec3& vec) const noexcept;
	void SetUniform(const std::string& name, const glm::vec4& vec) const noexcept;
	void SetUniform(const std::string& name, const glm::mat3& mat) const noexcept;
	void SetUniform(const std::string& name, const glm::mat4& mat) const noexcept;

	void SetUniform(const GLuint loc, const bool val) const noexcept;
	void SetUniform(const GLuint loc, const int val) const noexcept;
	void SetUniform(const GLuint loc, const GLuint val) const noexcept;
	void SetUniform(const GLuint loc, const size_t val) const noexcept;
	void SetUniform(const GLuint loc, const float val) const noexcept;
	void SetUniform(const GLuint loc, const glm::vec2& vec) const noexcept;
	void SetUniform(const GLuint loc, const glm::vec3& vec) const noexcept;
	void SetUniform(const GLuint loc, const glm::vec4& vec) const noexcept;
	void SetUniform(const GLuint loc, const glm::mat3& mat) const noexcept;
	void SetUniform(const GLuint loc, const glm::mat4& mat) const noexcept;

	void SetUniform(const std::string& name, const LightComp& light) const noexcept;
	void SetUniform(const std::string& name, const Material& mat) const noexcept;

	// @TODO: TEMP
	void SetUniform(const std::string& name, const TrifectaLight& light) const noexcept;


	template <typename T>
	void SetUniform(const std::string& name, const T* ptr) const
	{
		if (ptr)
			SetUniform(name, *ptr);
	}

	template <typename T>
	void SetUniform(const std::string& name, const std::vector<T>& vector) const
	{
		SetUniform(name + "Count", static_cast<int>(vector.size()));

		for (size_t i = 0; i < vector.size(); i++)
		{
			SetUniform(name + "[" + std::to_string(i) + "]", vector[i]);
		}
	}
#pragma endregion

protected:
	GLuint mID;
};

struct ShaderComp
{
	ShaderComp(const std::string& file);
	ShaderComp(const GLenum type, const std::string& code);

	GLuint mShaderID;
	GLenum mShaderType;

private:
	bool Create(const GLenum type, const std::string& code, std::string& infoLog);
	GLenum GetType(const std::string& file);
};

struct Shader : public ShaderBase
{
	friend class ResourceManager;

	Shader(const std::vector<ShaderComp>& comps);

	template <class T, class... Ts>
	Shader(T fb, Ts... rest) : Shader(InitializeShader(fb, rest)) {}
	Shader(const std::string& shaderFile) : Shader(InitializeShader(shaderFile)) { mFilePath = shaderFile; }

	~Shader();
	void Shutdown();

	std::string GetFilePath() { return mFilePath; }

protected:
	template <class T, class... Ts>
	std::vector<ShaderComp> InitializeShader(T fb, Ts... rest) const;
	std::vector<ShaderComp> InitializeShader(const std::string& file) const;

private:
	std::string mFilePath = "None";
};

template<class T, class ...Ts>
inline std::vector<ShaderComp> Shader::InitializeShader(T shader, Ts ...rest) const
{
	std::vector<ShaderComp> shaderList;
	shaderList.push_back();
	shaderList.insert(shaderList.end(), InitializeShader(rest...));
	return shaderList;
}

