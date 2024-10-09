#pragma once
#include "Misc/Singleton.h"

#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_mouse.h>
#include <glm/glm.hpp>

struct MouseState
{
	Uint32 mMouseMask[5];
	glm::ivec2 mPos;
};

class InputManager
{
	Singleton(InputManager)

	using KeyboardState = Uint8;

	enum eMouseButton
	{
		eButtonLeft   = 0,
		eButtonMiddle = 1,
		eButtonRight  = 2,
		eButtonX1     = 3,
		eButtonX2     = 4
	};

	void Initialize();
	void Shutdown();

	bool isKeyPressed(SDL_Scancode key); // @TODO: Does not work
	bool isKeyReleased(SDL_Scancode key);
	bool isKeyDown(SDL_Scancode key);
	bool isKeyUp(SDL_Scancode key);

	bool isButtonPressed(eMouseButton button);
	bool isButtonReleased(eMouseButton button);
	bool isButtonDown(eMouseButton button);
	bool isButtonUp(eMouseButton button);

	glm::ivec2 GetMouseMovement() { return mMouseMovement; }
	glm::ivec2 GetMousePos() { return mMouseState.mPos; }

	void SetMousePos(const glm::ivec2 & pos) { mMouseState.mPos = pos; SDL_WarpMouseGlobal(pos.x, pos.y); }
	void SetMousePos(const int w, const int h) { mMouseState.mPos = glm::ivec2(w, h); SDL_WarpMouseGlobal(w, h); }

	void ProcessInput();

private:
	enum eInputState
	{
		eInputUp	   = 0,      // 0000
		eInputDown	   = 1 << 0, // 0001
		eInputPressed  = 1 << 1, // 0010
		eInputReleased = 1 << 2  // 0100
	};

	KeyboardState mKeyboardState[SDL_NUM_SCANCODES];
	MouseState mMouseState;
	glm::ivec2 mMouseMovement;

	Uint8* mCurrentKeyState = nullptr;
	Uint32 mCurrentButtonState;
};

#define InputMgr InputManager::Instance()