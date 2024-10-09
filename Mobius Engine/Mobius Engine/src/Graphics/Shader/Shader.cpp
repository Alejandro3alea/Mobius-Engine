#include "Shader.h"
#include "Material.h"
#include "SceneNode.h"
#include "Misc/ColorConsole.h"
#include "Components/Graphics/LightComp.h"

#include <iostream>
#include <fstream>
#include <sstream>

// @TODO: TEMP
#define M_PI 3.141521f

ShaderComp::ShaderComp(const std::string& file) : mShaderType(GetType(file))
{
	std::ifstream tFile(file);

	if (!tFile.is_open() || !tFile.good())
	{
		PrintError("File not read in " + file);
		return;
	}

	std::stringstream shaderCode;
	char line[256];
	while (tFile.getline(line, 256))
		shaderCode << line << std::endl;

	std::string finalCode = shaderCode.str();

	std::string infoLog;
	if (!Create(mShaderType, finalCode, infoLog))
		PrintError("Compile failure in " + file + ": \n" + infoLog);
	else
		PrintSuccess("Shader " + file + " successfully read");
}

ShaderComp::ShaderComp(const GLenum type, const std::string& code) : mShaderType(type)
{
	std::string infoLog;
	if (!Create(type, code, infoLog))
	{
		// Get shader type
		std::string strShaderType;
		switch (mShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		case GL_TESS_CONTROL_SHADER: strShaderType = "tess control"; break;
		case GL_TESS_EVALUATION_SHADER: strShaderType = "tess eval"; break;
		}

		PrintError("Compile failure in custom " + strShaderType + " shader: \n" + infoLog);
	}
}


bool ShaderComp::Create(const GLenum type, const std::string& code, std::string& infoLog)
{
	// Create shader
	mShaderID = glCreateShader(type);

	const char* strFileData = code.c_str();
	glShaderSource(mShaderID, 1, &strFileData, NULL);

	glCompileShader(mShaderID);

	GLint status;
	glGetShaderiv(mShaderID, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(mShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(mShaderID, infoLogLength, NULL, strInfoLog);

		infoLog = strInfoLog;
		delete[] strInfoLog;
		return false;
	}
	else
		return true;
}

GLenum ShaderComp::GetType(const std::string& file)
{
	std::string ext = ResourceMgr->GetExtension(file);

	if (ext == "vert")
		return GL_VERTEX_SHADER;
	else if (ext == "frag")
		return GL_FRAGMENT_SHADER;
	else if (ext == "geom")
		return GL_GEOMETRY_SHADER;
	else if (ext == "comp")		// Unused
		return GL_COMPUTE_SHADER;
	else if (ext == "tesc")
		return GL_TESS_CONTROL_SHADER;
	else if (ext == "tese")
		return GL_TESS_EVALUATION_SHADER;
	else if (ext == "mesh")
		return GL_NONE;		// Unknown extension
	else if (ext == "task")
		return GL_NONE;		// Unknown extension
	else if (ext == "rgen")
		return GL_NONE;		// Unknown extension
	else if (ext == "rint")
		return GL_NONE;		// Unknown extension
	else if (ext == "rmiss")
		return GL_NONE;		// Unknown extension
	else if (ext == "rahit")
		return GL_NONE;		// Unknown extension
	else if (ext == "rchit")
		return GL_NONE;		// Unknown extension
	else if (ext == "rcall")
		return GL_NONE;		// Unknown extension
	else
	{
		PrintWarning("Unknown shader type specified. Maybe the file has a wrong extension?");
		return GL_NONE;
	}
}


///////////////////////////////////////////////////////////////////////////////////////
////////
///////////////////////////////////////////////////////////////////////////////////////


Shader::Shader(const std::vector<ShaderComp>& comps)
{
	mID = glCreateProgram();

	for (auto it : comps)
		glAttachShader(mID, it.mShaderID);

	glLinkProgram(mID);


	GLint status;
	glGetProgramiv(mID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(mID, GL_INFO_LOG_LENGTH, &infoLogLength);

		int infoSize = infoLogLength + 1;
		GLchar* strInfoLog = new GLchar[static_cast<size_t>(infoSize)];
		glGetProgramInfoLog(mID, infoLogLength, NULL, strInfoLog);
		PrintError("Linker failure: " + std::string(strInfoLog));
		delete[] strInfoLog;
	}

	for (auto it : comps)
		glDetachShader(mID, it.mShaderID);
}
	
Shader::~Shader()
{
	Shutdown();

	PrintDebug("Deleted Shader with ID " + std::to_string(mID));
}

void Shader::Shutdown()
{
}

std::vector<ShaderComp> Shader::InitializeShader(const std::string& file) const
{
	std::vector<ShaderComp> comps;

	std::ifstream tFile(file);

	if (!tFile.is_open() || !tFile.good())
		PrintError("File not read in " + file);

	char line[256];
	while (tFile.getline(line, 256))
	{
		std::stringstream sShaderFile;
		sShaderFile << line << std::endl;
		std::string shaderPath = sShaderFile.str();
		shaderPath.pop_back();
		comps.push_back(ShaderComp(shaderPath));
	}
	
	return comps;
}

#pragma region SetUniform
// NAMES
void ShaderBase::SetUniform(const std::string& name, const bool val) const noexcept
{
	glUniform1i(glGetUniformLocation(mID, name.c_str()), static_cast<int>(val));
}

void ShaderBase::SetUniform(const std::string& name, const int val) const noexcept
{
	glUniform1i(glGetUniformLocation(mID, name.c_str()), val);
}

void ShaderBase::SetUniform(const std::string& name, const GLuint val) const noexcept
{
	glUniform1i(glGetUniformLocation(mID, name.c_str()), val);
}

void ShaderBase::SetUniform(const std::string& name, const size_t val) const noexcept
{
	glUniform1i(glGetUniformLocation(mID, name.c_str()), val);
}

void ShaderBase::SetUniform(const std::string& name, const float val) const noexcept
{
	glUniform1f(glGetUniformLocation(mID, name.c_str()), val);
}

void ShaderBase::SetUniform(const std::string& name, const glm::vec2& vec) const noexcept
{
	glUniform2f(glGetUniformLocation(mID, name.c_str()), vec.x, vec.y);
}

void ShaderBase::SetUniform(const std::string& name, const glm::vec3& vec) const noexcept
{
	glUniform3f(glGetUniformLocation(mID, name.c_str()), vec.x, vec.y, vec.z);
}

void ShaderBase::SetUniform(const std::string& name, const glm::vec4& vec) const noexcept
{
	glUniform4f(glGetUniformLocation(mID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}

void ShaderBase::SetUniform(const std::string& name, const glm::mat3& mat) const noexcept
{
	glUniformMatrix3fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderBase::SetUniform(const std::string& name, const glm::mat4& mat) const noexcept
{
	glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

///////////////////////////////////////////////////////////////////////////////////////
////////
///////////////////////////////////////////////////////////////////////////////////////

// LOCATIONS
void ShaderBase::SetUniform(const GLuint loc, const bool val) const noexcept
{
	glUniform1i(loc, static_cast<int>(val));
}

void ShaderBase::SetUniform(const GLuint loc, const int val) const noexcept
{
	glUniform1i(loc, val);
}

void ShaderBase::SetUniform(const GLuint loc, const GLuint val) const noexcept
{
	glUniform1i(loc, val);
}

void ShaderBase::SetUniform(const GLuint loc, const size_t val) const noexcept
{
	glUniform1i(loc, val);
}

void ShaderBase::SetUniform(const GLuint loc, const float val) const noexcept
{
	glUniform1f(loc, val);
}

void ShaderBase::SetUniform(const GLuint loc, const glm::vec2& vec) const noexcept
{
	glUniform2f(loc, vec.x, vec.y);
}

void ShaderBase::SetUniform(const GLuint loc, const glm::vec3& vec) const noexcept
{
	glUniform3f(loc, vec.x, vec.y, vec.z);
}

void ShaderBase::SetUniform(const GLuint loc, const glm::vec4& vec) const noexcept
{
	glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void ShaderBase::SetUniform(const GLuint loc, const glm::mat3& mat) const noexcept
{
	glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
}

void ShaderBase::SetUniform(const GLuint loc, const glm::mat4& mat) const noexcept
{
	glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
}

///////////////////////////////////////////////////////////////////////////////////////
////////
///////////////////////////////////////////////////////////////////////////////////////

void ShaderBase::SetUniform(const std::string& name, const LightComp& light) const noexcept
{
	SetUniform(name + ".Pos", light.mOwner->worldTransform.pos);
	SetUniform(name + ".Color", light.mColor);
}

void ShaderBase::SetUniform(const std::string& name, const Material& mat) const noexcept
{
	SetUniform(name + ".Color", mat.mColor);
	SetUniform(name + ".Ambient", mat.mAmbient);
	SetUniform(name + ".Metallic", mat.mMetallic);
	SetUniform(name + ".Roughness", mat.mRoughness);

#pragma region Material_Textures

	SetUniform(name + ".AlbedoTex", 0);
	SetUniform(name + ".NormalTex", 1);
	SetUniform(name + ".MetallicTex", 2);
	SetUniform(name + ".RoughnessTex", 3);
	SetUniform(name + ".OclussionTex", 4);
	SetUniform(name + ".EmissiveTex", 5);

	bool useAlbedoTex = mat.mAlbedoTex != nullptr;
	SetUniform(name + ".UseAlbedoTex", useAlbedoTex);
	if (useAlbedoTex)
	{
		glActiveTexture(GL_TEXTURE0);
		mat.mAlbedoTex->get()->Bind();
	}

	bool useNormalTex = mat.mNormalTex != nullptr;
	SetUniform(name + ".UseNormalTex", useNormalTex);
	if (useNormalTex)
	{
		glActiveTexture(GL_TEXTURE1);
		mat.mNormalTex->get()->Bind();
	}

	bool useMetallicTex = mat.mMetallicTex != nullptr;
	SetUniform(name + ".UseMetallicTex", useMetallicTex);
	if (useMetallicTex)
	{
		glActiveTexture(GL_TEXTURE2);
		mat.mMetallicTex->get()->Bind();
	}

	bool useRoughnessTex = mat.mRoughnessTex != nullptr;
	SetUniform(name + ".UseRoughnessTex", useRoughnessTex);
	if (useRoughnessTex)
	{
		glActiveTexture(GL_TEXTURE3);
		mat.mRoughnessTex->get()->Bind();
	}

	bool useOclussionTex = mat.mOclussionTex != nullptr;
	SetUniform(name + ".UseOclussionTex", useOclussionTex);
	if (useOclussionTex)
	{
		glActiveTexture(GL_TEXTURE4);
		mat.mOclussionTex->get()->Bind();
	}

	bool useEmissiveTex = mat.mEmissiveTex != nullptr;
	SetUniform(name + ".UseEmissiveTex", useEmissiveTex);
	if (useEmissiveTex)
	{
		glActiveTexture(GL_TEXTURE5);
		mat.mEmissiveTex->get()->Bind();
	}
#pragma endregion 
}

#include "Editor/Tools/Trifecta/TrifectaLight.h"
void ShaderBase::SetUniform(const std::string& name, const TrifectaLight& light) const noexcept
{
	SetUniform(name + ".ambient", light.ambient);
	SetUniform(name + ".diffuse", light.diffuse);
	SetUniform(name + ".specular", light.specular);

	SetUniform(name + ".position", light.position);

	SetUniform(name + ".spot_falloff", light.spot_falloff);
	SetUniform(name + ".spot_innerAngle", static_cast<float>(M_PI) / 180.f * light.spot_innerAngle);
	SetUniform(name + ".spot_outerAngle", static_cast<float>(M_PI) / 180.f * light.spot_outerAngle);

	SetUniform(name + ".const_Att", light.const_Att);
	SetUniform(name + ".linear_Att", light.linear_Att);
	SetUniform(name + ".quadratic_Att", light.quadratic_Att);
}
#pragma endregion