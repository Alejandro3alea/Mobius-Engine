#include "WindowMgr.h"
#include "ColorConsole.h"

WindowManager* WindowManager::mpInstance;

Window::Window(const unsigned winWidth, const unsigned winHeight) : mSize(winWidth, winHeight)
{
	// Create an instance of an SDLWindow
	mWindow_SDL = SDL_CreateWindow("Mobius Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (mWindow_SDL == nullptr)
	{
		PrintColoredText(std::string("SDL_CreateWindow Error : ") + SDL_GetError(), ConsoleColor::Red);
		SDL_Quit();
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Create an SDL context for OpenGL
	mWinContext = SDL_GL_CreateContext(mWindow_SDL);
	if (mWinContext == nullptr)
	{
		SDL_DestroyWindow(mWindow_SDL);
		PrintColoredText(std::string("SDL_GL_CreateContext Error : ") + SDL_GetError(), ConsoleColor::Red);
		SDL_Quit();
		exit(1);
	}
	mAspectRatio = static_cast<float>(winWidth) / static_cast<float>(winHeight);

	// Enable/Disable V-Sync. This determines the cap of fps.
	SDL_GL_SetSwapInterval(1);

	SDL_MaximizeWindow(mWindow_SDL);
}

Window::~Window()
{
	SDL_GL_DeleteContext(mWinContext);
	SDL_DestroyWindow(mWindow_SDL);
	SDL_Quit();
}

glm::uvec2 Window::size()
{
	int windowWidth, windowHeight;
	SDL_GetWindowSize(mWindow_SDL, &windowWidth, &windowHeight);
	return glm::uvec2(windowWidth, windowHeight);
}

void WindowManager::Initialize()
{
	// Initialize SLD library
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		PrintColoredText(std::string("Could not initialize SDL : ") + SDL_GetError(), ConsoleColor::Red);
		exit(1);
	}

	mCurrentWindow = std::shared_ptr<Window>(new Window(1280, 720));
}

void WindowManager::Update()
{

}

void WindowManager::Shutdown()
{
}
