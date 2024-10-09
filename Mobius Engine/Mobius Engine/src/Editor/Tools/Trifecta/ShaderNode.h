#pragma once
#include "Graphics/Shader/Shader.h"
#include "Resources/ResourceMgr.h"
#include "NodeAttrib.h"

#include <vector>
#include <string>
#include <tuple>

using FloatAttrib = TNodeAttribute<float>;
using Vec3Attrib  = TNodeAttribute<glm::vec3>;
using Vec2Attrib  = TNodeAttribute<glm::vec2>;
using IntAttrib   = TNodeAttribute<int>;

struct Model;
struct Framebuffer;

struct ShaderNode
{
	~ShaderNode();

	virtual void Initialize() { NodeInitialize(); }
	virtual void OnGui(const unsigned idx);

	virtual void WriteNode(std::ofstream& result) = 0;

	virtual void UpdatePersonalShader() = 0;
	virtual void DrawShader() = 0;

protected:
	virtual void NodeInitialize(bool isBillboard = true);

	virtual const std::string GetPersonalVertexShader();
	Shader* mShader;

public:
	std::vector<NodeAttribute*> mInParams;
	std::vector<NodeAttribute*> mOutParams;

	std::string mNodeName;
	std::string mFunction;

	Framebuffer* mFB;

	bool isRendered, renderOutput;
};

struct OutputNode : public ShaderNode
{
	OutputNode() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override {}
	void WriteVertex(std::ofstream& result);
	void WriteFragment(std::ofstream& result);

	virtual const std::string GetPersonalVertexShader() override;
	virtual const std::string GetPersonalFragmentShader();

	virtual void UpdatePersonalShader();
	virtual void DrawShader();

	Resource<Model>* mModel;
	float tRot;
};

#pragma region Arithmetic Nodes

struct Vec3ToFloat : public ShaderNode
{
	Vec3ToFloat() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;

	virtual void UpdatePersonalShader() override;
	virtual void DrawShader() override;
};

struct FloatToVec3 : public ShaderNode
{
	FloatToVec3() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;

	virtual void UpdatePersonalShader() override;
	virtual void DrawShader() override;
};

#pragma region Float Nodes
struct AddFloatNode : public ShaderNode
{
	AddFloatNode() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;

	virtual void UpdatePersonalShader() override;
	virtual void DrawShader() override;
};

struct SubstractFloatNode : public ShaderNode
{
	SubstractFloatNode() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;

	virtual void UpdatePersonalShader() override;
	virtual void DrawShader() override;
};

struct IncreaseFloatNode : public ShaderNode
{
	IncreaseFloatNode() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;
};

struct DecreaseFloatNode : public ShaderNode
{
	DecreaseFloatNode() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;
};
#pragma endregion
#pragma region Vec3 Nodes
struct AddVec3Node : public ShaderNode
{
	AddVec3Node() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;

	virtual void UpdatePersonalShader() override;
	virtual void DrawShader() override;
};

struct SubstractVec3Node : public ShaderNode
{
	SubstractVec3Node() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;

	virtual void UpdatePersonalShader() override;
	virtual void DrawShader() override;
};
#pragma endregion

#pragma endregion

#pragma region Math Nodes
struct SinNode : public ShaderNode
{
	SinNode() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;
};

struct NormalizeVec3Node : public ShaderNode
{
	NormalizeVec3Node() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;
};
#pragma endregion

#pragma region Texture Nodes
struct PerlinNoiseNode : public ShaderNode
{
	PerlinNoiseNode() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;

	virtual void UpdatePersonalShader() override;
	virtual void DrawShader() override;
};

struct WorleyNoiseNode : public ShaderNode
{
	WorleyNoiseNode() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;

	virtual void UpdatePersonalShader() override;
	virtual void DrawShader() override;
};
#pragma endregion

#pragma region Texture Nodes
struct TimeNode : public ShaderNode
{
	TimeNode() { Initialize(); }

	virtual void Initialize() override;

	virtual void WriteNode(std::ofstream& result) override;

	virtual void UpdatePersonalShader() override;
	virtual void DrawShader() override;
}; 
#pragma endregion