#include "ShaderNode.h"
#include "TrifectaEditor.h"
#include "TrifectaLight.h"

#include "Graphics/GfxMgr.h"
#include "Graphics/Light.h"
#include "Graphics/Model/Model.h"
#include "Graphics/WindowMgr.h"
#include "Graphics/Framebuffer.h"
#include "Misc/Timer.h"

#include "Editor/imgui/imgui.h"
#include "Editor/imgui/imnodes.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <sstream>

ShaderNode::~ShaderNode()
{
	for (auto it : mInParams)
		delete it;

	for (auto it : mOutParams)
		delete it;

	delete mFB;
	
	if (mShader)
		delete mShader;
}

void ShaderNode::NodeInitialize(bool isBillboard)
{
	for (auto it : mInParams)
		it->parentNode = this;

	for (auto it : mOutParams)
		it->parentNode = this;

	glm::uvec2 size(400, 400);
	mFB = new Framebuffer(1, ColorBuffer(size));
	mFB->Initialize(!isBillboard);

	mShader = nullptr;

	UpdatePersonalShader();
}


void ShaderNode::OnGui(const unsigned idx)
{
	imnodes::BeginNode(idx);
	ImGui::Text(mNodeName.c_str());
	ImGui::NewLine();

	ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(1.0f / 3.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(1.0f / 3.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(1.0f / 3.0f, 0.8f, 0.8f));

	for (unsigned i = 0; i < mInParams.size(); i++)
	{
		std::string valName = "##" + std::to_string(mInParams[i]->id);
		imnodes::BeginInputAttribute(mInParams[i]->id, imnodes::PinShape_TriangleFilled);
		ImGui::Text(mInParams[i]->name.c_str());

		if (mInParams[i]->linkIdx == -1)
		{
			if (FloatAttrib* fltAttrib = dynamic_cast<FloatAttrib*>(mInParams[i]))
			{
				ImGui::SameLine();
				ImGui::PushItemWidth(50.0f);
				ImGui::DragFloat(valName.c_str(), &fltAttrib->val, 0.001f, 0.0f, 1.0f);
				ImGui::PopItemWidth();
			}
			else if (Vec3Attrib* vec3Attrib = dynamic_cast<Vec3Attrib*>(mInParams[i]))
			{
				ImGui::SameLine();
				ImGui::PushItemWidth(150.0f);
				ImGui::DragFloat3(valName.c_str(), &vec3Attrib->val[0], 0.001f, 0.0f, 1.0f);
				ImGui::PopItemWidth();
			}
			else if (Vec2Attrib* vec2Attrib = dynamic_cast<Vec2Attrib*>(mInParams[i]))
			{
				ImGui::SameLine();
				ImGui::PushItemWidth(150.0f);
				ImGui::DragFloat2(valName.c_str(), &vec2Attrib->val[0], 0.001f);
				ImGui::PopItemWidth();
			}
			else if (IntAttrib* intAttrib = dynamic_cast<IntAttrib*>(mInParams[i]))
			{
				ImGui::SameLine();
				ImGui::PushItemWidth(150.0f);
				if (ImGui::DragInt(valName.c_str(), &intAttrib->val, 0.01f, 0.0f))
				ImGui::PopItemWidth();
			}
		}

		imnodes::EndInputAttribute();
	}

	ImGui::NewLine();

	for (unsigned i = 0; i < mOutParams.size(); i++)
	{
		std::string valName = "##" + std::to_string(mOutParams[i]->id);
		imnodes::BeginOutputAttribute(mOutParams[i]->id, imnodes::PinShape_TriangleFilled);
		ImGui::Text(mOutParams[i]->name.c_str());
		imnodes::EndOutputAttribute();
	}

	ImGui::PopStyleColor(3);

	if (renderOutput)
		ImGui::Image((ImTextureID)mFB->GetTexID(), { 250, 250 }, { 0, 1 }, { 1, 0 });

	imnodes::EndNode();
}

const std::string ShaderNode::GetPersonalVertexShader()
{
	std::string result = "#version 400 core\n"
		"\n"
		"layout(location = 0) in vec3 vPos; \n"
		"layout(location = 1) in vec2 vTexCoords; \n"
		"\n"
		"// --------------------- OUT ---------------------\n"
		"out vec2 texUV; \n"
		"\n"
		"void main()\n"
		"{\n"
		"texUV = vTexCoords;\n"
		"gl_Position = vec4(vPos, 1.0f);\n"
		"}";

	return result;
}

void OutputNode::Initialize()
{
	mNodeName = "Output";
	mModel = ResourceMgr->Load<Model>("data/engine/meshes/obj/Suzanne.obj");
	tRot = 0.0f;

	// Input
	mInParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "Vertex Pos (3)", glm::vec3(0.0f, 0.0f, 0.0f)));	 // 0

	mInParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "Albedo (3)", glm::vec3(1.0f, 1.0f, 1.0f)));		 // 1
	mInParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "Normal (3)", glm::vec3(1.0f, 1.0f, 1.0f)));		 // 2
	mInParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "Emission (3)", glm::vec3(1.0f, 1.0f, 1.0f)));		 // 3

	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "Alpha (1)", 1.0f));							 // 4
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "Alpha Clip Threshold (1)", 0.0f));				 // 5
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "Metallic (1)", 1.0f));							 // 6

	ShaderNode::NodeInitialize(false);
}

void OutputNode::WriteVertex(std::ofstream& result)
{
	std::string vtxString;
	Vec3Attrib* vec3Attrib = dynamic_cast<Vec3Attrib*>(mInParams[0]);
	const int edgeIdx = vec3Attrib->linkIdx;

	if (edgeIdx == -1)
		vtxString = "vec4(vPos + vec3(" + std::to_string(vec3Attrib->val.x) + ", " + std::to_string(vec3Attrib->val.y) + ", " + std::to_string(vec3Attrib->val.z) + "), 1.0f);";
	else
	{
		int tIdx = Trifecta->mEdges[edgeIdx].to;
		reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(tIdx)->parentNode)->WriteNode(result);
		vtxString = "vec4(vPos + p" + std::to_string(edgeIdx) + ", 1.0f);";
	}

	result << "gl_Position = " + vtxString << std::endl;
}

void OutputNode::WriteFragment(std::ofstream& result)
{
	std::string fragString;
	Vec3Attrib* vec3Attrib = dynamic_cast<Vec3Attrib*>(mInParams[1]);
	const int edgeIdx = vec3Attrib->linkIdx;

	if (edgeIdx == -1)
		fragString = "vec4(vPos + vec3(" + std::to_string(vec3Attrib->val.x) + ", " + std::to_string(vec3Attrib->val.y) + ", " + std::to_string(vec3Attrib->val.z) + "), 1.0f);";
	else
	{
		int tIdx = Trifecta->mEdges[edgeIdx].to;
		reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(tIdx)->parentNode)->WriteNode(result);
		fragString = "vec4(vPos + p" + std::to_string(edgeIdx) + ", 1.0f);";
	}

	result << "FragColor = " + fragString << std::endl;
}

const std::string OutputNode::GetPersonalVertexShader()
{
	std::stringstream vtxStr;

	Vec3Attrib* vtxPosAttrib = dynamic_cast<Vec3Attrib*>(mInParams[0]);
	const bool paramLinked = vtxPosAttrib->linkIdx == -1;
	const bool inputsRendered = paramLinked || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[vtxPosAttrib->linkIdx].from)->parentNode)->renderOutput;

	vtxStr << "#version 400 core" << std::endl;
	vtxStr << "" << std::endl;
	vtxStr << "layout (location = 0) in vec3 vPos;" << std::endl;
	vtxStr << "layout (location = 1) in vec2 vTexCoord;" << std::endl;
	vtxStr << "layout (location = 2) in vec3 vNormal;" << std::endl;
	vtxStr << "layout (location = 3) in vec3 vTangent;" << std::endl;
	vtxStr << "layout (location = 4) in vec3 vBitangent;" << std::endl;
	vtxStr << "" << std::endl;
	vtxStr << "struct Light {" << std::endl;
	vtxStr << "int type;" << std::endl;
	vtxStr << "vec3 ambient;" << std::endl;
	vtxStr << "vec3 diffuse;" << std::endl;
	vtxStr << "vec3 specular;" << std::endl;
	vtxStr << "vec3 position;" << std::endl;
	vtxStr << "float spot_falloff;" << std::endl;
	vtxStr << "float spot_innerAngle;" << std::endl;
	vtxStr << "float spot_outerAngle;" << std::endl;
	vtxStr << "float const_Att;" << std::endl;
	vtxStr << "float linear_Att;" << std::endl;
	vtxStr << "float quadratic_Att;" << std::endl;
	vtxStr << "};" << std::endl;
	vtxStr << "" << std::endl;
	vtxStr << "// ------------------- UNIFORM -------------------"		<< std::endl;
	vtxStr << "uniform Light uLight;"					<< std::endl;
	if (inputsRendered)
		vtxStr << "uniform vec3 uTextureData0;" << std::endl;
	else
		vtxStr << "uniform sampler2D uTextureData0;" << std::endl;
	vtxStr << ""														<< std::endl;
	vtxStr << "uniform mat4 model;"										<< std::endl;
	vtxStr << "uniform mat4 projView;"									<< std::endl;
	vtxStr << ""														<< std::endl;
	vtxStr << "uniform vec3 uViewPos;"									<< std::endl;
	vtxStr << "// --------------------- OUT ---------------------"		<< std::endl;
	vtxStr << "out OUT_IN_VARIABLES {"									<< std::endl;
	vtxStr << "	vec2 texUV;"											<< std::endl;
	vtxStr << "	vec3 vertexPos;"										<< std::endl;
	vtxStr << "	vec3 fragPos;"											<< std::endl;
	vtxStr << "	vec3 normal;"											<< std::endl;
	vtxStr << "	vec3 tan_lightPos;"										<< std::endl;
	vtxStr << "	vec3 tan_viewPos;"										<< std::endl;
	vtxStr << "	vec3 tan_surfacePos;"									<< std::endl;
	vtxStr << "} outVar;"												<< std::endl;
	vtxStr << ""														<< std::endl;
	vtxStr << "void main()"												<< std::endl;
	vtxStr << "{"														<< std::endl;
	vtxStr << "	outVar.texUV = vTexCoord;"								<< std::endl;
	vtxStr << "	outVar.vertexPos = vPos;"								<< std::endl;
	vtxStr << "	outVar.fragPos = (model * vec4(vPos, 1.0f)).xyz;"		<< std::endl;
	vtxStr << ""														<< std::endl;
	vtxStr << "	// Compute tangent matrix"								<< std::endl;
	vtxStr << "    mat3 normalMatrix = transpose(inverse(mat3(model)));" << std::endl;
	vtxStr << "    vec3 T = normalize(normalMatrix * vTangent);"		<< std::endl;
	vtxStr << "    vec3 N = normalize(normalMatrix * vNormal);"			<< std::endl;
	vtxStr << "    T = normalize(T - dot(T, N) * N);"					<< std::endl;
	vtxStr << "    vec3 B = cross(N, T);"								<< std::endl;
	vtxStr << ""														<< std::endl;
	vtxStr << "	mat3 TBN = transpose(mat3(T, B, N));"					<< std::endl;
	vtxStr << ""														<< std::endl;
	vtxStr << "	"														<< std::endl;
	vtxStr << "	// compute all light positions in tangent space"		<< std::endl;
	vtxStr << "	outVar.tan_lightPos = TBN * uLight.position;"			<< std::endl;
	vtxStr << "	outVar.tan_viewPos = TBN * uViewPos;"					<< std::endl;
	vtxStr << "	outVar.tan_surfacePos = TBN * outVar.fragPos;"			<< std::endl;
	vtxStr << "	outVar.normal = vNormal;"								<< std::endl;
	vtxStr << ""														<< std::endl;
	if (inputsRendered)
		vtxStr << "	gl_Position = projView * model * vec4(vPos + uTextureData0, 1.0f);" << std::endl;
	else
		vtxStr << "	gl_Position = projView * model * vec4(vPos + texture(uTextureData0, vTexCoord).rgb, 1.0f);" << std::endl;
	
	vtxStr << "}"														<< std::endl;

	return vtxStr.str();
}

const std::string OutputNode::GetPersonalFragmentShader()
{
	std::stringstream fragStr;
	
	Vec3Attrib* vec3Attribs[3] = { dynamic_cast<Vec3Attrib*>(mInParams[1]), dynamic_cast<Vec3Attrib*>(mInParams[2]), dynamic_cast<Vec3Attrib*>(mInParams[3]) };
	FloatAttrib* floatAttribs[3] = { dynamic_cast<FloatAttrib*>(mInParams[4]), dynamic_cast<FloatAttrib*>(mInParams[5]), dynamic_cast<FloatAttrib*>(mInParams[6]) };
	const bool paramLinked[6] = { vec3Attribs[0]->linkIdx == -1, vec3Attribs[1]->linkIdx == -1, vec3Attribs[2]->linkIdx == -1, floatAttribs[0]->linkIdx == -1, floatAttribs[1]->linkIdx == -1, floatAttribs[2]->linkIdx == -1 };
	const bool inputsRendered[6] = { paramLinked[0] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[vec3Attribs[0]->linkIdx].from)->parentNode)->renderOutput,
									 paramLinked[1] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[vec3Attribs[1]->linkIdx].from)->parentNode)->renderOutput,
									 paramLinked[2] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[vec3Attribs[2]->linkIdx].from)->parentNode)->renderOutput,
									 paramLinked[3] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[floatAttribs[0]->linkIdx].from)->parentNode)->renderOutput,
									 paramLinked[4] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[floatAttribs[1]->linkIdx].from)->parentNode)->renderOutput,
									 paramLinked[5] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[floatAttribs[2]->linkIdx].from)->parentNode)->renderOutput };

	fragStr << "#version 400 core" << std::endl;
	fragStr << "" << std::endl;
	fragStr << "#define POINT_LIGHT 0" << std::endl;
	fragStr << "#define DIRECTIONAL_LIGHT 1" << std::endl;
	fragStr << "#define SPOT_LIGHT 2" << std::endl;
	fragStr << "#define LIGHT_NUM_MAX 7" << std::endl;
	fragStr << "" << std::endl;

	fragStr << "struct Light {" << std::endl;
	fragStr << "int type;" << std::endl;
	fragStr << "vec3 ambient;" << std::endl;
	fragStr << "vec3 diffuse;" << std::endl;
	fragStr << "vec3 specular;" << std::endl;
	fragStr << "vec3 position;" << std::endl;
	fragStr << "float spot_falloff;" << std::endl;
	fragStr << "float spot_innerAngle;" << std::endl;
	fragStr << "float spot_outerAngle;" << std::endl;
	fragStr << "float const_Att;" << std::endl;
	fragStr << "float linear_Att;" << std::endl;
	fragStr << "float quadratic_Att;" << std::endl;
	fragStr << "};" << std::endl;

	fragStr << "" << std::endl;
	fragStr << "// --------------------- IN ---------------------" << std::endl;
	fragStr << "in OUT_IN_VARIABLES{" << std::endl;
	fragStr << "	vec2 texUV;" << std::endl;
	fragStr << "	vec3 vertexPos;" << std::endl;
	fragStr << "	vec3 fragPos;" << std::endl;
	fragStr << "	vec3 normal;" << std::endl;
	fragStr << "	vec3 tan_lightPos;" << std::endl;
	fragStr << "	vec3 tan_viewPos;" << std::endl;
	fragStr << "	vec3 tan_surfacePos;" << std::endl;
	fragStr << "} inVar;" << std::endl;
	fragStr << "" << std::endl;
	fragStr << "// --------------------- OUT --------------------" << std::endl;
	fragStr << "out vec4 FragColor;" << std::endl;
	fragStr << "" << std::endl;
	fragStr << "// ------------------- UNIFORM ------------------" << std::endl;
	fragStr << "uniform Light uLight;" << std::endl;
	fragStr << "uniform vec3 uViewPos;" << std::endl;
	fragStr << "" << std::endl;
	for (unsigned int i = 0; i < 3; i++)
	{
		if (inputsRendered[i])
			fragStr << "uniform vec3 uTextureData" << std::to_string(i + 1) << ";" << std::endl;
		else
			fragStr << "uniform sampler2D uTextureData" << std::to_string(i + 1) << ";" << std::endl;
	}

	for (unsigned int i = 0; i < 3; i++)
	{
		if (inputsRendered[i])
			fragStr << "uniform float uTextureData" << std::to_string(i + 4) << ";" << std::endl;
		else
			fragStr << "uniform sampler2D uTextureData" << std::to_string(i + 4) << ";" << std::endl;
	}
	fragStr << "" << std::endl;
	fragStr << "void main()" << std::endl;
	fragStr << "{" << std::endl;

	if (inputsRendered[0])
		fragStr << "	vec3 diffuseTexel = uTextureData1;" << std::endl;
	else
		fragStr << "	vec3 diffuseTexel = texture(uTextureData1, inVar.texUV).rgb;" << std::endl;

	if (inputsRendered[5])
		fragStr << "	float specularTexel = uTextureData6;" << std::endl;
	else
		fragStr << "	float specularTexel = texture(uTextureData6, inVar.texUV).r;" << std::endl;

	if (inputsRendered[1])
		fragStr << "	vec3 normalTexel = vec3(0.5, 0.5, 1.0);" << std::endl;
	else
		fragStr << "	vec3 normalTexel = texture(uTextureData2, inVar.texUV).rgb;" << std::endl;

	fragStr << "	normalTexel = normalize(normalTexel * 2.0f - 1.0f);" << std::endl;
	fragStr << "" << std::endl;
	fragStr << "	vec3 finalColor = vec3(0.0f, 0.0f, 0.0f);" << std::endl;
	fragStr << "" << std::endl;
	fragStr << "	// ambient" << std::endl;
	fragStr << "	vec3 ambient = uLight.ambient * diffuseTexel;" << std::endl;
	fragStr << "	" << std::endl;
	fragStr << "	// diffuse " << std::endl;
	fragStr << "	vec3 lightDir = normalize(inVar.tan_lightPos - inVar.fragPos);" << std::endl;
	fragStr << "	float diff = max(dot(lightDir, normalTexel), 0.0f);" << std::endl;
	fragStr << "	vec3 diffuse = uLight.diffuse * diff * diffuseTexel;  " << std::endl;
	fragStr << "	" << std::endl;
	fragStr << "	// specular" << std::endl;
	fragStr << "	vec3 viewDir = normalize(inVar.tan_viewPos - inVar.fragPos);" << std::endl;
	fragStr << "	vec3 reflectDir = reflect(-lightDir, normalTexel);  " << std::endl;
	fragStr << "	vec3 halfwayDir = normalize(lightDir + viewDir);" << std::endl;
	fragStr << "	float spec = pow(max(dot(normalTexel, halfwayDir), 0.0f), 32.0f); // @TODO: Shinyness" << std::endl;
	fragStr << "	vec3 specular = uLight.specular * spec * specularTexel;  " << std::endl;
	
	fragStr << "	" << std::endl;
	fragStr << "	// @TODO: Light casting types  " << std::endl;
	fragStr << "	    " << std::endl;
	fragStr << "	finalColor += ambient + diffuse + specular;" << std::endl;
	fragStr << "	" << std::endl;
	if (inputsRendered[3])
		fragStr << "	FragColor = vec4(finalColor, uTextureData4);" << std::endl;
	else
		fragStr << "	FragColor = vec4(finalColor, texture(uTextureData4, inVar.texUV).r);" << std::endl;
	fragStr << "}" << std::endl;

	return fragStr.str();
}

void OutputNode::UpdatePersonalShader()
{
	std::vector<ShaderComp> shaderList;
	shaderList.push_back(ShaderComp(GL_VERTEX_SHADER, GetPersonalVertexShader()));
	shaderList.push_back(ShaderComp(GL_FRAGMENT_SHADER, GetPersonalFragmentShader()));

	if (mShader) delete mShader;
	mShader = new Shader(shaderList);
}

void OutputNode::DrawShader()
{
	isRendered = true;
	tRot += TimeMgr->deltaTime;

	for (auto it : mInParams)
		if (it->linkIdx != -1)
			reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[it->linkIdx].from)->parentNode)->DrawShader();

	Vec3Attrib* vec3Attrib[4] = { dynamic_cast<Vec3Attrib*>(mInParams[0]), dynamic_cast<Vec3Attrib*>(mInParams[1]), dynamic_cast<Vec3Attrib*>(mInParams[2]), dynamic_cast<Vec3Attrib*>(mInParams[3]) };
	FloatAttrib* floatAttrib[3] = { dynamic_cast<FloatAttrib*>(mInParams[4]), dynamic_cast<FloatAttrib*>(mInParams[5]), dynamic_cast<FloatAttrib*>(mInParams[6]) };
	const int paramIdx[7] = { vec3Attrib[0]->linkIdx, vec3Attrib[1]->linkIdx , vec3Attrib[2]->linkIdx , vec3Attrib[3]->linkIdx , floatAttrib[0]->linkIdx , floatAttrib[1]->linkIdx , floatAttrib[2]->linkIdx };
	const bool paramLinked[7] = { paramIdx[0] == -1, paramIdx[1] == -1, paramIdx[2] == -1, paramIdx[3] == -1, paramIdx[4] == -1, paramIdx[5] == -1, paramIdx[6] == -1 };

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, mFB->BufferSize().x, mFB->BufferSize().y);
	mFB->BindFramebuffer();
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mShader->Bind();

		TrifectaLight tLight;

		glm::mat4 model(1.0f);
		model = glm::rotate(model, tRot, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 view = glm::lookAt(glm::vec3(0,2,5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 proj = glm::perspective(glm::radians(45.f), 1.0f, 0.1f, 100.f);
		glm::mat4 projView = proj * view;

		mShader->SetUniform("uLight", tLight);
		mShader->SetUniform("uViewPos", glm::vec3(0.0f, 2.0f, 5.0f));
		mShader->SetUniform("model", model);
		mShader->SetUniform("projView", projView);

		for (unsigned i = 0; i < 4; i++)
		{
			if (paramLinked[i])
			{
				mShader->SetUniform("uTextureData" + std::to_string(i), vec3Attrib[i]->val);
			}
			else
			{
				ShaderNode* node = reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[vec3Attrib[i]->linkIdx].from)->parentNode);
				if (!node->renderOutput)
				{
					Vec2Attrib* fromVal = dynamic_cast<Vec2Attrib*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[vec3Attrib[i]->linkIdx].from));
					if (fromVal)
						mShader->SetUniform("uTextureData" + std::to_string(i), fromVal->val);
				}
				else
				{
					mShader->SetUniform("uTextureData" + std::to_string(i), 1 + i);
					glActiveTexture(GL_TEXTURE1 + i);
					int tIdx = Trifecta->mEdges[paramIdx[i]].from;
					reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(tIdx)->parentNode)->mFB->BindTexture();
				}
			}
		}	
		for (unsigned i = 4; i < 7; i++)
		{
			if (paramLinked[i])
			{
				mShader->SetUniform("uTextureData" + std::to_string(i), floatAttrib[i - 4]->val);
			}
			else
			{
				ShaderNode* node = reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[floatAttrib[i - 4]->linkIdx].from)->parentNode);
				if (!node->renderOutput)
				{
					Vec2Attrib* fromVal = dynamic_cast<Vec2Attrib*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[floatAttrib[i - 4]->linkIdx].from));
					mShader->SetUniform("uTextureData" + std::to_string(i), fromVal->val);
				}
				else
				{
					mShader->SetUniform("uTextureData" + std::to_string(i), 1 + i);
					glActiveTexture(GL_TEXTURE1 + i);
					int tIdx = Trifecta->mEdges[paramIdx[i]].from;
					reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(tIdx)->parentNode)->mFB->BindTexture();
				}
			}
		}

		glCullFace(GL_FRONT);
		mModel->get()->Render(mShader);
		glCullFace(GL_BACK);
		mModel->get()->Render(mShader);
	}
	mFB->UnbindFramebuffer();
	glDisable(GL_BLEND);
}


void FloatToVec3::Initialize()	
{
	mNodeName = "float to Vec3";
	renderOutput = false;

	// Input
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "x", 0.0f));
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "y", 0.0f));
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "z", 0.0f));

	// Output
	mOutParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "out", glm::vec3(0.0f, 0.0f, 0.0f)));;

	ShaderNode::Initialize();

}

void FloatToVec3::WriteNode(std::ofstream& result)
{

}

void FloatToVec3::UpdatePersonalShader()
{
	std::stringstream result;

	FloatAttrib* paramAttrib[3] = { dynamic_cast<FloatAttrib*>(mInParams[0]), dynamic_cast<FloatAttrib*>(mInParams[1]), dynamic_cast<FloatAttrib*>(mInParams[2]) };
	const bool paramLinked[3] = { paramAttrib[0]->linkIdx == -1, paramAttrib[1]->linkIdx == -1, paramAttrib[2]->linkIdx == -1 };
	const bool inputsRendered[3] = { paramLinked[0] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[0]->linkIdx].from)->parentNode)->renderOutput,
									 paramLinked[1] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[1]->linkIdx].from)->parentNode)->renderOutput,
									 paramLinked[2] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[2]->linkIdx].from)->parentNode)->renderOutput };

	result << "#version 400 core" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- IN ---------------------" << std::endl;
	result << "in vec2 texUV;" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- OUT --------------------" << std::endl;
	result << "out vec4 FragColor;" << std::endl;
	result << "" << std::endl;
	result << "// ------------------- UNIFORM ------------------" << std::endl;
	for (unsigned int i = 0; i < 3; i++)
	{
		if (inputsRendered[i])
			result << "uniform float uTextureData" << std::to_string(i) << ";" << std::endl;
		else
			result << "uniform sampler2D uTextureData" << std::to_string(i) << ";" << std::endl;
	}
	result << "" << std::endl;
	result << "void main()" << std::endl;
	result << "{" << std::endl;

	std::string paramInputs[3];
	for (unsigned int i = 0; i < 3; i++)
	{
		if (inputsRendered[i])
			paramInputs[i] = "uTextureData" + std::to_string(i);
		else
			paramInputs[i] = "texture(uTextureData" + std::to_string(i) + ", texUV).r";
	}

	result << "FragColor = vec4(" << paramInputs[0] << ", " <<  paramInputs[1] << ", " << paramInputs[2] << ", 1.0f);" << std::endl;
	result << "}";

	std::vector<ShaderComp> shaderList;
	shaderList.push_back(ShaderComp(GL_VERTEX_SHADER, GetPersonalVertexShader()));
	shaderList.push_back(ShaderComp(GL_FRAGMENT_SHADER, result.str()));

	if (mShader) delete mShader;
	mShader = new Shader(shaderList);
}

void FloatToVec3::DrawShader()
{
	isRendered = true;

	for (auto it : mInParams)
		if (it->linkIdx != -1)
			reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[it->linkIdx].from)->parentNode)->DrawShader();

	FloatAttrib* paramAttrib[3] = { dynamic_cast<FloatAttrib*>(mInParams[0]), dynamic_cast<FloatAttrib*>(mInParams[1]), dynamic_cast<FloatAttrib*>(mInParams[2]) };
	const int paramIdx[3] = { paramAttrib[0]->linkIdx, paramAttrib[1]->linkIdx, paramAttrib[2]->linkIdx };
	const bool paramLinked[3] = { paramIdx[0] == -1, paramIdx[1] == -1, paramIdx[2] == -1 };

	glViewport(0, 0, mFB->BufferSize().x, mFB->BufferSize().y);
	mFB->BindFramebuffer();
	{
		glClear(GL_COLOR_BUFFER_BIT);

		mShader->Bind();

		for (unsigned i = 0; i < 3; i++)
		{
			if (paramLinked[i])
			{
				mShader->SetUniform("uTextureData" + std::to_string(i), paramAttrib[i]->val);
			}
			else 
			{
				ShaderNode* node = reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[i]->linkIdx].from)->parentNode);
				if (!node->renderOutput)
				{
					FloatAttrib* fromVal = dynamic_cast<FloatAttrib*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[i]->linkIdx].from));
					mShader->SetUniform("uTextureData" + std::to_string(i), fromVal->val);
				}
				else
				{
					mShader->SetUniform("uTextureData" + std::to_string(i), 1 + i);
					glActiveTexture(GL_TEXTURE1 + i);
					int tIdx = Trifecta->mEdges[paramIdx[i]].from;
					reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(tIdx)->parentNode)->mFB->BindTexture();
				}
			}
		}

		GfxMgr->mDefaultShapes["Quad"]->get()->Render(mShader);
	}
	mFB->UnbindFramebuffer();
}

void Vec3ToFloat::Initialize()
{
	mNodeName = "Vec3 to float";

	// Input
	mInParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "in", glm::vec3(0.0f, 0.0f, 0.0f)));

	// Output
	mOutParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "out", 0.0f));;

	ShaderNode::Initialize();
}

void Vec3ToFloat::WriteNode(std::ofstream& result)
{

}

void Vec3ToFloat::UpdatePersonalShader()
{
}

void Vec3ToFloat::DrawShader()
{
}

void AddFloatNode::Initialize()
{
	mNodeName = "Add (1)";

	// Input
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "lhs", 0.0f));
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "rhs", 0.0f));

	// Output
	mOutParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "out", 0.0f));;

	ShaderNode::Initialize();
}

void AddFloatNode::WriteNode(std::ofstream& result)
{
}

void AddFloatNode::UpdatePersonalShader()
{
	std::stringstream result;

	FloatAttrib* paramAttrib[2] = { dynamic_cast<FloatAttrib*>(mInParams[0]), dynamic_cast<FloatAttrib*>(mInParams[1]) };
	const bool paramLinked[2] = { paramAttrib[0]->linkIdx == -1, paramAttrib[1]->linkIdx == -1 };

	result << "#version 400 core" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- IN ---------------------" << std::endl;
	result << "in vec2 texUV;" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- OUT --------------------" << std::endl;
	result << "out vec4 FragColor;" << std::endl;
	result << "" << std::endl;
	result << "// ------------------- UNIFORM ------------------" << std::endl;
	for (unsigned int i = 0; i < 2; i++)
	{
		if (paramLinked[i])
			result << "uniform sampler2D uTextureData" << std::to_string(i) << ";" << std::endl;
		else
			result << "uniform vec3 uTextureData" << std::to_string(i) << ";" << std::endl;
	}
	result << "" << std::endl;
	result << "void main()" << std::endl;
	result << "{" << std::endl;

	std::string paramInputs[2];
	for (unsigned int i = 0; i < 2; i++)
	{
		if (paramLinked[i])
			paramInputs[i] = "uTextureData" + std::to_string(i);
		else
			paramInputs[i] = "texture(uTextureData" + std::to_string(i) + ", texUV).rgb";
	}

	result << "FragColor = vec4(" << paramInputs[0] << " + " << paramInputs[1] << ", 1.0f);" << std::endl;
	result << "}";

	std::vector<ShaderComp> shaderList;
	shaderList.push_back(ShaderComp(GL_VERTEX_SHADER, GetPersonalVertexShader()));
	shaderList.push_back(ShaderComp(GL_FRAGMENT_SHADER, result.str()));

	if (mShader) delete mShader;
	mShader = new Shader(shaderList);
}

void AddFloatNode::DrawShader()
{
}

void SubstractFloatNode::Initialize()
{
	mNodeName = "Substract (1)";

	// Input
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "lhs", 0.0f));
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "rhs", 0.0f));

	// Output
	mOutParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "out", 0.0f));;

	ShaderNode::Initialize();
}

void SubstractFloatNode::WriteNode(std::ofstream& result)
{
	
}


void SubstractFloatNode::UpdatePersonalShader()
{
	std::stringstream result;

	FloatAttrib* paramAttrib[2] = { dynamic_cast<FloatAttrib*>(mInParams[0]), dynamic_cast<FloatAttrib*>(mInParams[1]) };
	const bool paramLinked[2] = { paramAttrib[0]->linkIdx == -1, paramAttrib[1]->linkIdx == -1 };

	result << "#version 400 core" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- IN ---------------------" << std::endl;
	result << "in vec2 texUV;" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- OUT --------------------" << std::endl;
	result << "out vec4 FragColor;" << std::endl;
	result << "" << std::endl;
	result << "// ------------------- UNIFORM ------------------" << std::endl;
	for (unsigned int i = 0; i < 2; i++)
	{
		if (paramLinked[i])
			result << "uniform sampler2D uTextureData" << std::to_string(i) << ";" << std::endl;
		else
			result << "uniform vec3 uTextureData" << std::to_string(i) << ";" << std::endl;
	}
	result << "" << std::endl;
	result << "void main()" << std::endl;
	result << "{" << std::endl;

	std::string paramInputs[2];
	for (unsigned int i = 0; i < 2; i++)
	{
		if (paramLinked[i])
			paramInputs[i] = "uTextureData" + std::to_string(i);
		else
			paramInputs[i] = "texture(uTextureData" + std::to_string(i) + ", texUV).rgb";
	}

	result << "FragColor = vec4(" << paramInputs[0] << " - " << paramInputs[1] << ", 1.0f);" << std::endl;
	result << "}";

	std::vector<ShaderComp> shaderList;
	shaderList.push_back(ShaderComp(GL_VERTEX_SHADER, GetPersonalVertexShader()));
	shaderList.push_back(ShaderComp(GL_FRAGMENT_SHADER, result.str()));

	if (mShader) delete mShader;
	mShader = new Shader(shaderList);
}

void SubstractFloatNode::DrawShader()
{
}

void IncreaseFloatNode::Initialize()
{

	ShaderNode::Initialize();
}

void IncreaseFloatNode::WriteNode(std::ofstream& result)
{
}

void DecreaseFloatNode::Initialize()
{

	ShaderNode::Initialize();
}

void DecreaseFloatNode::WriteNode(std::ofstream& result)
{
}


void AddVec3Node::Initialize()
{
	mNodeName = "Add (3)";
	renderOutput = true;

	// Input
	mInParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "lhs", glm::vec3(0.0f)));
	mInParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "rhs", glm::vec3(0.0f)));

	// Output
	mOutParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "out", glm::vec3(0.0f)));;

	ShaderNode::Initialize();
}

void AddVec3Node::WriteNode(std::ofstream& result)
{
	std::string paramString[2];
	Vec3Attrib* paramAttrib[2] = { dynamic_cast<Vec3Attrib*>(mInParams[0]), dynamic_cast<Vec3Attrib*>(mInParams[1]) };
	const int paramIdx[2] = { paramAttrib[0]->linkIdx, paramAttrib[1]->linkIdx };
	const int outIdx = mOutParams[0]->linkIdx;

	for (unsigned int i = 0; i < 2; i++)
	{
		if (paramIdx[i] == -1)
			paramString[i] = "vec3(" + std::to_string(paramAttrib[i]->val.x) + ", " + std::to_string(paramAttrib[i]->val.y) + ", " + std::to_string(paramAttrib[i]->val.z) + ")";
		else
		{
			int tIdx = Trifecta->mEdges[paramIdx[i]].from;
			reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(tIdx)->parentNode)->WriteNode(result);
			paramString[i] = "p" + std::to_string(paramIdx[i]);
		}
	}

	result << "vec3 p" + std::to_string(outIdx) +  " = " + paramString[0] + " + " + paramString[1] + ";" << std::endl;
}

void AddVec3Node::UpdatePersonalShader()
{
	std::stringstream result;
	
	Vec3Attrib* paramAttrib[2] = { dynamic_cast<Vec3Attrib*>(mInParams[0]), dynamic_cast<Vec3Attrib*>(mInParams[1]) };
	const bool paramLinked[2] = { paramAttrib[0]->linkIdx == -1, paramAttrib[1]->linkIdx == -1 };
	const bool inputsRendered[2] = { paramLinked[0] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[0]->linkIdx].from)->parentNode)->isRendered,
									 paramLinked[1] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[1]->linkIdx].from)->parentNode)->isRendered };

	result << "#version 400 core" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- IN ---------------------" << std::endl;
	result << "in vec2 texUV;" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- OUT --------------------" << std::endl;
	result << "out vec4 FragColor;" << std::endl;
	result << "" << std::endl;
	result << "// ------------------- UNIFORM ------------------" << std::endl;
	for (unsigned int i = 0; i < 2; i++)
	{
		if (inputsRendered[i])
			result << "uniform vec3 uTextureData" << std::to_string(i) << ";" << std::endl;
		else
			result << "uniform sampler2D uTextureData" << std::to_string(i) << ";" << std::endl;
	}
	result << "" << std::endl;
	result << "void main()" << std::endl;
	result << "{" << std::endl;

	std::string paramInputs[2];
	for (unsigned int i = 0; i < 2; i++)
	{
		if (inputsRendered[i])
			paramInputs[i] = "uTextureData" + std::to_string(i);
		else
			paramInputs[i] = "texture(uTextureData" + std::to_string(i) + ", texUV).rgb";
	}

	result << "FragColor = vec4(" << paramInputs[0] << " + " << paramInputs[1] << ", 1.0f);" << std::endl;
	result << "}";

	std::vector<ShaderComp> shaderList;
	shaderList.push_back(ShaderComp(GL_VERTEX_SHADER, GetPersonalVertexShader()));
	shaderList.push_back(ShaderComp(GL_FRAGMENT_SHADER, result.str()));

	if (mShader) delete mShader;
	mShader = new Shader(shaderList);
}

void AddVec3Node::DrawShader()
{
	isRendered = true;

	for (auto it : mInParams)
		if (it->linkIdx != -1)
			reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[it->linkIdx].from)->parentNode)->DrawShader();

	Vec3Attrib* paramAttrib[2] = { dynamic_cast<Vec3Attrib*>(mInParams[0]), dynamic_cast<Vec3Attrib*>(mInParams[1]) };
	const int paramIdx[2]	   = { paramAttrib[0]->linkIdx, paramAttrib[1]->linkIdx };
	const bool paramLinked[2]  = { paramIdx[0] == -1, paramIdx[1] == -1 };
	const bool inputsRendered[2] = { paramLinked[0] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[0]->linkIdx].from)->parentNode)->isRendered,
									 paramLinked[1] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[1]->linkIdx].from)->parentNode)->isRendered };

	glViewport(0, 0, mFB->BufferSize().x, mFB->BufferSize().y);
	mFB->BindFramebuffer();
	{
		glClear(GL_COLOR_BUFFER_BIT);

		mShader->Bind();

		for (unsigned i = 0; i < 2; i++)
		{
			if (inputsRendered[i])
			{
				mShader->SetUniform("uTextureData" + std::to_string(i), paramAttrib[i]->val);
			}
			else
			{
				mShader->SetUniform("uTextureData" + std::to_string(i), 1 + i);
				glActiveTexture(GL_TEXTURE1 + i);
				int tIdx = Trifecta->mEdges[paramIdx[i]].from;
				reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(tIdx)->parentNode)->mFB->BindTexture();
			}
		}

		GfxMgr->mDefaultShapes["Quad"]->get()->Render(mShader);
	}
	mFB->UnbindFramebuffer();
}

void SubstractVec3Node::Initialize()
{
	mNodeName = "Substract (3)";
	renderOutput = true;

	// Input
	mInParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "lhs", glm::vec3(0.0f)));
	mInParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "rhs", glm::vec3(0.0f)));

	// Output
	mOutParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "out", glm::vec3(0.0f)));;

	ShaderNode::Initialize();
}

void SubstractVec3Node::WriteNode(std::ofstream& result)
{
	std::string paramString[2];
	Vec3Attrib* paramAttrib[2] = { dynamic_cast<Vec3Attrib*>(mInParams[0]), dynamic_cast<Vec3Attrib*>(mInParams[1]) };
	const int paramIdx[2] = { paramAttrib[0]->linkIdx, paramAttrib[1]->linkIdx };
	const int outIdx = mOutParams[0]->linkIdx;

	for (unsigned int i = 0; i < 2; i++)
	{
		if (paramIdx[i] == -1)
			paramString[i] = "vec3(" + std::to_string(paramAttrib[i]->val.x) + ", " + std::to_string(paramAttrib[i]->val.y) + ", " + std::to_string(paramAttrib[i]->val.z) + ")";
		else
		{
			int tIdx = Trifecta->mEdges[paramIdx[i]].from;
			reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(tIdx)->parentNode)->WriteNode(result);
			paramString[i] = "p" + std::to_string(paramIdx[i]);
		}
	}

	result << "vec3 p" + std::to_string(outIdx) + " = " + paramString[0] + " - " + paramString[1] + ";" << std::endl;
}

void SubstractVec3Node::UpdatePersonalShader()
{
	std::stringstream result;

	Vec3Attrib* paramAttrib[2] = { dynamic_cast<Vec3Attrib*>(mInParams[0]), dynamic_cast<Vec3Attrib*>(mInParams[1]) };
	const bool paramLinked[2] = { paramAttrib[0]->linkIdx == -1, paramAttrib[1]->linkIdx == -1 };
	const bool inputsRendered[2] = { paramLinked[0] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[0]->linkIdx].from)->parentNode)->isRendered,
									 paramLinked[1] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[1]->linkIdx].from)->parentNode)->isRendered };

	result << "#version 400 core" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- IN ---------------------" << std::endl;
	result << "in vec2 texUV;" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- OUT --------------------" << std::endl;
	result << "out vec4 FragColor;" << std::endl;
	result << "" << std::endl;
	result << "// ------------------- UNIFORM ------------------" << std::endl;
	for (unsigned int i = 0; i < 2; i++)
	{
		if (inputsRendered[i])
			result << "uniform vec3 uTextureData" << std::to_string(i) << ";" << std::endl;
		else
			result << "uniform sampler2D uTextureData" << std::to_string(i) << ";" << std::endl;
	}
	result << "" << std::endl;
	result << "void main()" << std::endl;
	result << "{" << std::endl;

	std::string paramInputs[2];
	for (unsigned int i = 0; i < 2; i++)
	{
		if (inputsRendered[i])
			paramInputs[i] = "uTextureData" + std::to_string(i);
		else
			paramInputs[i] = "texture(uTextureData" + std::to_string(i) + ", texUV).rgb";
	}

	result << "FragColor = vec4(" << paramInputs[0] << " - " << paramInputs[1] << ", 1.0f);" << std::endl;
	result << "}";

	std::vector<ShaderComp> shaderList;
	shaderList.push_back(ShaderComp(GL_VERTEX_SHADER, GetPersonalVertexShader()));
	shaderList.push_back(ShaderComp(GL_FRAGMENT_SHADER, result.str()));

	if (mShader) delete mShader;
	mShader = new Shader(shaderList);
}

void SubstractVec3Node::DrawShader()
{
	isRendered = true;

	for (auto it : mInParams)
		if (it->linkIdx != -1)
			reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[it->linkIdx].from)->parentNode)->DrawShader();

	Vec3Attrib* paramAttrib[2] = { dynamic_cast<Vec3Attrib*>(mInParams[0]), dynamic_cast<Vec3Attrib*>(mInParams[1]) };
	const int paramIdx[2] = { paramAttrib[0]->linkIdx, paramAttrib[1]->linkIdx };
	const bool paramLinked[2] = { paramIdx[0] == -1, paramIdx[1] == -1 };
	const bool inputsRendered[2] = { paramLinked[0] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[0]->linkIdx].from)->parentNode)->renderOutput,
									 paramLinked[1] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[1]->linkIdx].from)->parentNode)->renderOutput };

	glViewport(0, 0, mFB->BufferSize().x, mFB->BufferSize().y);
	mFB->BindFramebuffer();
	{
		glClear(GL_COLOR_BUFFER_BIT);

		for (unsigned i = 0; i < 2; i++)
		{
			if (paramLinked[i])
			{
				mShader->SetUniform("uTextureData" + std::to_string(i), paramAttrib[i]->val);
			}
			else
			{
				ShaderNode* node = reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[i]->linkIdx].from)->parentNode);
				if (!node->renderOutput)
				{
					Vec3Attrib* fromVal = dynamic_cast<Vec3Attrib*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[i]->linkIdx].from));
					mShader->SetUniform("uTextureData" + std::to_string(i), fromVal->val);
				}
				else
				{
					mShader->SetUniform("uTextureData" + std::to_string(i), 1 + i);
					glActiveTexture(GL_TEXTURE1 + i);
					int tIdx = Trifecta->mEdges[paramIdx[i]].from;
					reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(tIdx)->parentNode)->mFB->BindTexture();
				}
			}
		}

		GfxMgr->mDefaultShapes["Quad"]->get()->Render(mShader);
	}
	mFB->UnbindFramebuffer();
}
#pragma endregion

#pragma region Math Nodes
void SinNode::Initialize()
{
	mNodeName = "Sin";

	// Input
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "in", 0.0f));

	// Output
	mOutParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "out", 0.0f));;

	ShaderNode::Initialize();
}

void SinNode::WriteNode(std::ofstream& result)
{
}
#pragma endregion

#pragma region Texture Nodes
void PerlinNoiseNode::Initialize()
{
	mNodeName = "Perlin Noise";
	renderOutput = true;

	// Input
	mInParams.push_back(new Vec2Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC2, "Noise offset (2)", glm::vec2(0.0f)));
	mInParams.push_back(new Vec2Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC2, "Noise size (2)", glm::vec2(5.0f)));

	// Output
	mOutParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "out", glm::vec3(0.0f)));;

	ShaderNode::Initialize();
}

void PerlinNoiseNode::WriteNode(std::ofstream& result)
{
}

void PerlinNoiseNode::UpdatePersonalShader()
{
	std::stringstream result;

	Vec2Attrib* paramAttrib[2] = { dynamic_cast<Vec2Attrib*>(mInParams[0]), dynamic_cast<Vec2Attrib*>(mInParams[1]) };
	const bool paramLinked[2] = { paramAttrib[0]->linkIdx == -1, paramAttrib[1]->linkIdx == -1 };
	const bool inputsRendered[2] = { paramLinked[0] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[0]->linkIdx].from)->parentNode)->renderOutput,
									 paramLinked[1] || !reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[1]->linkIdx].from)->parentNode)->renderOutput };

	result << "#version 400 core" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- IN ---------------------" << std::endl;
	result << "in vec2 texUV;" << std::endl;
	result << "" << std::endl;
	result << "// --------------------- OUT --------------------" << std::endl;
	result << "out vec4 FragColor;" << std::endl;
	result << "" << std::endl;
	result << "// ------------------- UNIFORM ------------------" << std::endl;
	for (unsigned int i = 0; i < 2; i++)
	{
		if (inputsRendered[i])
			result << "uniform vec2 uTextureData" << std::to_string(i) << ";" << std::endl;
		else
			result << "uniform sampler2D uTextureData" << std::to_string(i) << ";" << std::endl;
	}
	result << "" << std::endl;
	result << "vec2 rand2(vec2 vec) { vec = vec2(dot(vec, vec2(127.1, 311.7)), dot(vec, vec2(269.5, 183.3))); return -1.0 + 2.0 * fract(sin(vec) * 43758.5453123); }" << std::endl;
	result << "" << std::endl;
	result << "float noise(vec2 vec) { vec2 i = floor(vec); vec2 f = fract(vec); vec2 smoothness = f * f * (3.0f - 2.0f * f);" << std::endl;
	result << "return mix( mix( dot( rand2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) )," << std::endl;
	result << "dot( rand2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), smoothness.x)," << std::endl;
	result << "mix( dot( rand2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) )," << std::endl;
	result << "dot( rand2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), smoothness.x), smoothness.y); }" << std::endl;
	result << "" << std::endl;
	result << "void main()" << std::endl;
	result << "{" << std::endl;

	std::string paramInputs[2];
	for (unsigned int i = 0; i < 2; i++)
	{
		if (inputsRendered[i])
			paramInputs[i] = "uTextureData" + std::to_string(i);
		else
			paramInputs[i] = "texture(uTextureData" + std::to_string(i) + ", texUV).rgb";
	}

	
	result << "float var = noise((texUV + " << paramInputs[0] << ") * " << paramInputs[1] << ");" << std::endl;
	result << "vec3 noiseTexel = vec3(var * 0.5f + 0.5f);" << std::endl;
	result << "FragColor = vec4(noiseTexel, 1.0f);" << std::endl;
	result << "}";

	std::vector<ShaderComp> shaderList;
	shaderList.push_back(ShaderComp(GL_VERTEX_SHADER, GetPersonalVertexShader()));
	shaderList.push_back(ShaderComp(GL_FRAGMENT_SHADER, result.str()));

	if (mShader) delete mShader;
	mShader = new Shader(shaderList);
}

void PerlinNoiseNode::DrawShader()
{
	isRendered = true;

	for (auto it : mInParams)
		if (it->linkIdx != -1)
			reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[it->linkIdx].from)->parentNode)->DrawShader();

	Vec2Attrib* paramAttrib[2] = { dynamic_cast<Vec2Attrib*>(mInParams[0]), dynamic_cast<Vec2Attrib*>(mInParams[1]) };
	const int paramIdx[2] = { paramAttrib[0]->linkIdx, paramAttrib[1]->linkIdx };
	const bool paramLinked[2] = { paramIdx[0] == -1, paramIdx[1] == -1 };

	glViewport(0, 0, mFB->BufferSize().x, mFB->BufferSize().y);
	mFB->BindFramebuffer();
	{
		glClear(GL_COLOR_BUFFER_BIT);

		mShader->Bind();

		for (unsigned i = 0; i < 2; i++)
		{
			if (paramLinked[i])
			{
				mShader->SetUniform("uTextureData" + std::to_string(i), paramAttrib[i]->val);
			}
			else
			{
				ShaderNode* node = reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[i]->linkIdx].from)->parentNode);
				if (!node->renderOutput)
				{
					Vec2Attrib* fromVal = dynamic_cast<Vec2Attrib*>(Trifecta->GetOutputAttribute(Trifecta->mEdges[paramAttrib[i]->linkIdx].from));
					mShader->SetUniform("uTextureData" + std::to_string(i), fromVal->val);
				}
				else
				{
					mShader->SetUniform("uTextureData" + std::to_string(i), 1 + i);
					glActiveTexture(GL_TEXTURE1 + i);
					int tIdx = Trifecta->mEdges[paramIdx[i]].from;
					reinterpret_cast<ShaderNode*>(Trifecta->GetOutputAttribute(tIdx)->parentNode)->mFB->BindTexture();
				}
			}
		}

		GfxMgr->mDefaultShapes["Quad"]->get()->Render(mShader);
	}
	mFB->UnbindFramebuffer();
}

void WorleyNoiseNode::Initialize()
{
	mNodeName = "Worley Noise";
	renderOutput = true;

	// Input
	mInParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "in", 0.0f));

	// Output
	mOutParams.push_back(new Vec3Attrib(Trifecta->GetAttribIdx(), ATTRIB_VEC3, "out", glm::vec3(0.0f)));;

	ShaderNode::Initialize();
}

void WorleyNoiseNode::WriteNode(std::ofstream& result)
{
}

void WorleyNoiseNode::UpdatePersonalShader()
{
}

void WorleyNoiseNode::DrawShader()
{

}
#pragma endregion

#pragma region Texture Nodes
void TimeNode::Initialize()
{
	mNodeName = "Time";
	renderOutput = false;

	// Output
	mOutParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "Time (1)", 0.0f));
	mOutParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "Sin Time (1)", 0.0f));
	mOutParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "Cos Time (1)", 0.0f));
	mOutParams.push_back(new FloatAttrib(Trifecta->GetAttribIdx(), ATTRIB_FLOAT, "Delta Time (1)", 0.0f));

	ShaderNode::Initialize();
}

void TimeNode::WriteNode(std::ofstream& result)
{

}

void TimeNode::UpdatePersonalShader()
{

}

void TimeNode::DrawShader()
{
	auto& timeVal = dynamic_cast<FloatAttrib*>(mOutParams[0])->val;
	timeVal += TimeMgr->deltaTime;
	dynamic_cast<FloatAttrib*>(mOutParams[1])->val  = std::sin(timeVal);
	dynamic_cast<FloatAttrib*>(mOutParams[2])->val  = std::cos(timeVal);
	dynamic_cast<FloatAttrib*>(mOutParams[3])->val  = TimeMgr->deltaTime;

	isRendered = true;
}
#pragma endregion