#pragma comment(lib, "SHELL32.LIB")

#include "Composition/SceneGraph.h"
#include "Resources/ResourceMgr.h"
#include "Composition/Factory.h"
#include "Physics/PhysicsMgr.h"
#include "Graphics/WindowMgr.h"
#include "Graphics/GfxMgr.h"
#include "Audio/AudioMgr.h"
#include "Input/InputMgr.h"
#include "Editor/Editor.h"
#include "Misc/Timer.h"

#include "Editor/Tools/Profiler/Profiler.h"

#undef main
int main(void)
{
	WindowMgr->Initialize();
	GfxMgr->Initialize();
	AudioMgr->Initialize();
	PhysicsMgr->Initialize();
	ResourceMgr->Initialize();
	Factory->Initialize();
	Editor->Initialize();
	InputMgr->Initialize();
	Scene->NewScene();

	GfxMgr->Load();
	Editor->Load();

	while (!WindowMgr->mCurrentWindow->isClosed())
	{
		TimeMgr->StartFrame();

		Profiler->FrameStart();

		Profiler->ProfileStart("Input");
		InputMgr->ProcessInput();
		Profiler->ProfileEnd("Input");

		if (Editor->IsPlaying())
		{
			// Game Loop
			// Game Logic

			Profiler->ProfileStart("Physics");
			PhysicsMgr->Update();
			Profiler->ProfileEnd("Physics");
		}

		Profiler->ProfileStart("Graphics");
		GfxMgr->Update();
		GfxMgr->Render();
		Profiler->ProfileEnd("Graphics");

		Profiler->ProfileStart("Editor");
		Editor->Update();
		Profiler->ProfileEnd("Editor");

		Profiler->ProfileStart("Audio");
		AudioMgr->Update();
		Profiler->ProfileEnd("Audio");

		Profiler->ProfileStart("Swap");
		SDL_GL_SwapWindow(*WindowMgr->mCurrentWindow);
		Profiler->ProfileEnd("Swap");

		Profiler->FrameEnd();

		TimeMgr->EndFrame();
	}

	Scene->Clear();
	InputMgr->Shutdown();
	Editor->Shutdown();
	ResourceMgr->Shutdown();
	WindowMgr->Shutdown();
	AudioMgr->Shutdown();
	GfxMgr->Shutdown();

	return 0;
}