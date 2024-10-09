#pragma once
#include "Singleton.h"

#include <chrono>

class TimeManager
{
	Singleton(TimeManager);

public:
	void StartFrame();
	void EndFrame();

public:
	float deltaTime = 0.016667f;
	float fixedDeltaTime = 0.016667f;

private:
	std::chrono::steady_clock::time_point mStartTime;
};

#define TimeMgr TimeManager::Instance()