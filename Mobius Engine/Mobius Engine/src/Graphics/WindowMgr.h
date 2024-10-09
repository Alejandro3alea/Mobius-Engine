#pragma once
#include "Misc/Singleton.h"

#include <GL/glew.h>
#include <GL/GL.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include <windows.h>
#include <memory>

struct Window
{
	Window(const unsigned winWidth, const unsigned winHeight);
	~Window();

	void Close() { mbIsClosed = true; }
	bool isClosed() { return mbIsClosed; }

	glm::uvec2 size();
	float aspectRatio() { return mAspectRatio; }

	operator SDL_GLContext() const { return mWinContext; }
	operator SDL_Window*() const { return mWindow_SDL; }

private:
	glm::uvec2 mSize;
	SDL_Window* mWindow_SDL;
	SDL_GLContext mWinContext;
	float mAspectRatio;

	bool mbIsClosed = false;
};

class WindowManager
{
	Singleton(WindowManager);

	void Initialize();
	void Update();
	void Shutdown();

public:
	std::shared_ptr<Window> mCurrentWindow;
};

#define WindowMgr WindowManager::Instance()