#pragma once
#include "Singleton.h"
#include "ShaderNode.h"
#include "NodeEdge.h"
#include "Editor/Tools/EditorTool.h"

#include <map>

class TrifectaEditor : public EditorTool
{
	Singleton(TrifectaEditor)

	virtual void Initialize() final;
	virtual void Update() final;
	virtual void Shutdown() final;

	void ProcessShader();
	
	const unsigned GetAttribIdx() { return mAttribIdx++; }
	const unsigned GetEdgeIdx() { return mEdgeIdx++; }

	NodeAttribute* GetInputAttribute(const unsigned idx);
	NodeAttribute* GetOutputAttribute(const unsigned idx);

private:
	void DrawNodes();

	void ProcessVertex();
	void ProcessFragment();

	// ------------------- VARIABLES -----------------------
public:
	std::vector<ShaderNode*> mNodes;
	std::map<int, NodeEdge> mEdges;

private:
	unsigned mAttribIdx;
	unsigned mEdgeIdx;
};

#define Trifecta TrifectaEditor::Instance()