#include "Editor.h"
#include "WindowMgr.h"
#include "Serialization.h"
#include "InputMgr.h"
#include "GfxMgr.h"
#include "ImGuiWidgets.h"

#include "Tools/Trifecta/TrifectaEditor.h"
#include "Tools/ContentBrowser/ContentBrowser.h"
#include "Tools/Profiler/Profiler.h"

#include "Components/Graphics/ModelRenderer.h"
#include "Components/Graphics/FurRenderer.h"
#include "Components/Graphics/LightComp.h"
#include "Components/Graphics/ParticleEmitter.h"

#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/ImGuizmo.h"
#include <filesystem>

#undef LoadIcon

ImGuiEditor* ImGuiEditor::mpInstance;

#define MAX_EDIT_CHAR_COUNT 256
#define STR_TO_CHAR_ARR(str, resultName, charCount) char resultName[charCount]; \
for (unsigned i = 0; i < mEditText.size(); i++) \
	resultName[i] = mEditText[i];						\
resultName[mEditText.size()] = '\0'						

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::Initialize()
{
	const char* glsl_version = "#version 330";

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		  // Enable docking (See ImGui Docking tutorial)

	mDockspaceFlags |= ImGuiDockNodeFlags_NoDockingInCentralNode;
	mDockspaceFlags |= ImGuiDockNodeFlags_PassthruCentralNode;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	Window* win = WindowMgr->mCurrentWindow.get();
	// Setup Platform/Renderer bindings

	ImGui_ImplSDL2_InitForOpenGL(*win, win);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Tools:
	mTools.push_back(Trifecta);
	mTools.push_back(ContentBrowser);
	mTools.push_back(Profiler);

	for (EditorTool* it : mTools)
		it->Initialize();


	mWindowOpenChecks["PostProcessSettings"] = false;

	mModalChecks["Save"] = false;
	mModalChecks["Credits"] = false;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::Load()
{
	// Components
	LoadIcon("Transform");
	LoadIcon("MeshRenderer");
	LoadIcon("RigidBody");
	LoadIcon("Light");
	LoadIcon("Particles");
	LoadIcon("Gravity");
	LoadIcon("Bill");
	LoadIcon("HitMark");
	LoadIcon("FurRenderer");
	LoadIcon("Snip");
	LoadIcon("Map");
	LoadIcon("Podium");
	LoadIcon("Download");
	LoadIcon("Upload");
	LoadIcon("Robot");
	LoadIcon("Justify");
	LoadIcon("AudioEmitter");
	LoadIcon("AudioListener");

	// File types
	LoadIcon("Folder");
	LoadIcon("Unknown");
	LoadIcon("AudioFile");
	LoadIcon("Material");
	LoadIcon("MapFile");
	LoadIcon("Sky");
	LoadIcon("HDR");
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::Update()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	Window* win = WindowMgr->mCurrentWindow.get();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(*win);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	Dockspace();
	MainMenu();

	Scene->OnGui();

	if (mSelectedNode)
		mSelectedNode->OnGui();

	for (auto it : mTools)
		if (it->IsEnabled())
			it->Update();

	ImGui::EndFrame();

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (InputMgr->isKeyPressed(SDL_SCANCODE_F5))
	{
		if (Editor->IsPlaying())
			Editor->Stop();
		else
			Editor->Play();
	}
	if (Editor->IsPlaying() && InputMgr->isKeyPressed(SDL_SCANCODE_F6))
		Editor->Pause();

	if (mDragAndDropPayload != nullptr && !ImGui::IsMouseDown(0))
	{
		mOnDropWidget.Broadcast(mDragAndDropPayload);
		mOnDropWidget.Clear();
		mDragAndDropPayload = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::Pause()
{
	mIsPlaying = false;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::Stop()
{
	mIsPlaying = false;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::Play()
{
	mIsPlaying = true;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool ImGuiEditor::IsPlaying()
{
	return mIsPlaying;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::Dockspace()
{
	// DockSpace
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &mbDockWindowOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("TestDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), mDockspaceFlags);
	}

	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::MainMenu()
{
	static bool ShowImGuiDemoWindow;
	static bool isSavingNewScene;

	if (ImGui::BeginMainMenuBar()) // MainMenu (top-horizontal bar)
	{
		if (ImGui::BeginMenu("Menu")) // MainMenu block
		{
			if (ImGui::BeginMenu("New")) // MainMenu block Item
			{
				if (ImGui::BeginMenu("GameObject")) // display spaces list
				{
					if (ImGui::MenuItem("Create"))
					{
						Scene->CreateNode();
					}

					if (ImGui::MenuItem("Cube"))
					{
						SceneNode& modelNode = Scene->CreateNode();
						modelNode.AddComp(new ModelRenderer("data/engine/meshes/obj/Cube.obj"));
					}

					if (ImGui::MenuItem("Sphere"))
					{
						SceneNode& modelNode = Scene->CreateNode();
						modelNode.AddComp(new ModelRenderer("data/engine/meshes/obj/Sphere.obj"));
					}

					if (ImGui::MenuItem("Suzanne"))
					{
						SceneNode& modelNode = Scene->CreateNode();
						modelNode.AddComp(new ModelRenderer("data/engine/meshes/obj/Suzanne.obj"));
					}

					if (ImGui::MenuItem("PBRTest"))
					{
						SceneNode& modelNode = Scene->CreateNode();
						modelNode.AddComp(new ModelRenderer("data/engine/meshes/obj/PBRTest.obj"));
					}

					if (ImGui::MenuItem("Atmosphere"))
					{
						SceneNode& modelNode = Scene->CreateNode();
						modelNode.AddComp(new ModelRenderer("data/engine/meshes/obj/Sphere.obj"));
					}

					if (ImGui::MenuItem("Fur"))
					{
						SceneNode& modelNode = Scene->CreateNode();
						modelNode.AddComp(new FurRenderer());
					}

					if (ImGui::MenuItem("Light"))
					{
						SceneNode& lightNode = Scene->CreateNode();
						lightNode.AddComp(new LightComp());
					}

					if (ImGui::MenuItem("Particle System"))
					{
						SceneNode& lightNode = Scene->CreateNode();
						lightNode.AddComp(new ParticleEmitter());
					}

					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Scene")) // display spaces list
					Scene->NewScene();

				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save"))
			{
				mModalChecks["Save"] = true;
			}
			if (ImGui::MenuItem("Open..."))
			{
				json Val = FileToJson("temp.json");
				Scene->NewScene();
				Scene->FromJson(Val);
			}
 
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Trifecta"))
				Trifecta->Enable();
			if (ImGui::MenuItem("Profiler"))
				Profiler->Enable();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::Button("ImGui Demo Window"))
				ShowImGuiDemoWindow = !ShowImGuiDemoWindow;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Play"))
		{
			Editor->Play();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Stop"))
		{
			Editor->Stop();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Pause"))
		{
			Editor->Pause();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Gfx"))
		{
			if (ImGui::Checkbox("Wireframe mode", &GfxMgr->mWireframeMode))
			{
				GfxMgr->OnWireframeModeChanged();
			}

			if (ImGui::Button("Post Process Settings"))
				mWindowOpenChecks["PostProcessSettings"] = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("About"))
		{
			if (ImGui::Button("Credits"))
				mModalChecks["Credits"] = true;

			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	HandleEditorPopups();

	if (ShowImGuiDemoWindow)
		ImGui::ShowDemoWindow();
	if (mWindowOpenChecks["PostProcessSettings"])
		ShowPostProcessSettings();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::ShowPostProcessSettings()
{
	ImGui::Begin("Post Process Settings");

	if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Bloom"))
		{
			ImGui::OnGui("Enable Bloom", GfxMgr->mBloomEnabled);
			ImGui::OnGui("Threshold", GfxMgr->mBloomThreshold);
			ImGui::OnGui("Knee", GfxMgr->mBloomKnee);
			ImGui::OnGui("Intensity", GfxMgr->mBloomIntensity);
			ImGui::OnGui("Dirt Intensity", GfxMgr->mBloomDirtIntensity);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Gamma & Exposure"))
		{
			ImGui::OnGui("Gamma", GfxMgr->mGamma);
			ImGui::OnGui("Exposure", GfxMgr->mExposure);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::HandleEditorPopups()
{
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (mModalChecks["Save"])
	{
		ImGui::OpenPopup("Save");
		mModalChecks["Save"] = false;
	}

	if (ImGui::BeginPopupModal("Save", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Save level");
		ImGui::Separator();

		const std::string SceneName = Scene->mName;
		const unsigned MaxCharCount = 256;
		char c[MaxCharCount];
		for (unsigned i = 0; i < SceneName.size(); i++)
			c[i] = SceneName[i];
		c[SceneName.size()] = '\0';
		if (ImGui::InputText(".json", c, MaxCharCount))
			Scene->mName = std::string(c);

		if (ImGui::Button("Save"))
		{
			std::string SceneName = Scene->GetName();
			json Val;
			Scene->ToJson(Val);
			JsonToFile(Val, SceneName + ".json");
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	//////////////////////////////////////////////////////////////////////////
	// 
	//////////////////////////////////////////////////////////////////////////

	if (mModalChecks["Credits"])
	{
		ImGui::OpenPopup("Credits");
		mModalChecks["Credits"] = false;
	}

	if (ImGui::BeginPopupModal("Credits", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Libraries:");
		ImGui::Text("Graphics - glew (OpenGL)");
		ImGui::Text("Model loading - assimp");
		ImGui::Text("Audio - OpenAL");
		ImGui::Text("Physics - PhysX");
		ImGui::Text("Windows - SDL2");

		ImGui::NewLine();

		ImGui::Text("Icons by"); 
		ImGui::SameLine(); 
		ImGui::TextColored(ImVec4(0, 1, 1, 1), "icons8.com");

		if (ImGui::IsItemClicked())
		{
			// Use the system command to open a URL
			std::string command = "start " + std::string("https://icons8.com/");
			std::system(command.c_str());
		}

		ImGui::NewLine();

		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}

	//////////////////////////////////////////////////////////////////////////
	// 
	//////////////////////////////////////////////////////////////////////////

	if (mModalChecks["CreateFolder"])
	{
		ImGui::OpenPopup("CreateFolder");
		mModalChecks["CreateFolder"] = false;
		mEditText = "";
	}

	if (ImGui::BeginPopupModal("CreateFolder", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Create folder");
		ImGui::Separator();

		STR_TO_CHAR_ARR(mEditText, c, MAX_EDIT_CHAR_COUNT);

		if (ImGui::InputText("##foldername", c, MAX_EDIT_CHAR_COUNT))
			mEditText = std::string(c);

		if (ImGui::Button("Create"))
		{
			std::string folderPath = ContentBrowser->GetPath() + "/" + mEditText;
			std::filesystem::create_directory(folderPath);
			ContentBrowser->mOnContentBrowserUpdate.Broadcast(ContentBrowser->GetPath());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	//////////////////////////////////////////////////////////////////////////
	// 
	//////////////////////////////////////////////////////////////////////////

	if (mModalChecks["CreateLevel"])
	{
		ImGui::OpenPopup("CreateLevel");
		mModalChecks["CreateLevel"] = false;
		mEditText = "";
	}

	if (ImGui::BeginPopupModal("CreateLevel", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Create level");
		ImGui::Separator();

		STR_TO_CHAR_ARR(mEditText, c, MAX_EDIT_CHAR_COUNT);

		if (ImGui::InputText(".level", c, MAX_EDIT_CHAR_COUNT))
			mEditText = std::string(c);

		if (ImGui::Button("Create"))
		{
			std::string filePath = ContentBrowser->GetPath() + "/" + mEditText + ".level";
			JsonToFile(Scene->CreateNewJson(filePath), filePath);
			ContentBrowser->mOnContentBrowserUpdate.Broadcast(ContentBrowser->GetPath());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	//////////////////////////////////////////////////////////////////////////
	// 
	//////////////////////////////////////////////////////////////////////////

	if (mModalChecks["CreateMaterial"])
	{
		ImGui::OpenPopup("CreateMaterial");
		mModalChecks["CreateMaterial"] = false;
		mEditText = "";
	}

	if (ImGui::BeginPopupModal("CreateMaterial", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Create material");
		ImGui::Separator();

		STR_TO_CHAR_ARR(mEditText, c, MAX_EDIT_CHAR_COUNT);

		if (ImGui::InputText(".mat", c, MAX_EDIT_CHAR_COUNT))
			mEditText = std::string(c);

		if (ImGui::Button("Create"))
		{
			std::string folderPath = ContentBrowser->GetPath() + "/" + mEditText;

			ContentBrowser->mOnContentBrowserUpdate.Broadcast(ContentBrowser->GetPath());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::SetModalFlagEnabled(const std::string& modalName)
{
	mModalChecks[modalName] = true;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::SetDragAndDropPayload(void* payload, const unsigned size)
{
	ImGui::SetDragDropPayload("ITEM_PAYLOAD", payload, sizeof(int)); // Set payload with the index
	mDragAndDropPayload = payload;
	mOnDragWidget.Broadcast();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ImGuiEditor::LoadIcon(const std::string& name)
{
	mIcons[name] = ResourceMgr->Load<Texture>("data/engine/icons/" + name + ".png");
}
