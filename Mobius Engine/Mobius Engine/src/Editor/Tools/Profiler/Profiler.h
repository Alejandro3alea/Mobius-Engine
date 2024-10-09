#pragma once
#include "Editor/Tools/EditorTool.h"
#include "Singleton.h"

#include <map>
#include <string>
#include <array>
#include <Windows.h>

#include "imgui/imgui.h"
#include "Json/json.hpp"
using json = nlohmann::json;


// Size variable used to get Frame data count to be stored
#define FRAME_COUNT 60

struct ProfilerTool : public EditorTool
{
	Singleton(ProfilerTool)

	virtual void Initialize() final;
	virtual void Update() final;

	// Profile fuctions
	void ProfileStart(const std::string& name);
	void ProfileEnd(const std::string& name);

	// Frame data functions
	void FrameStart();
	void FrameEnd();


	// Frame capture functions
	void CaptureFrame() { mbCapture = true; }

	enum CaptureMode
	{
		eSingleFrame,
		eRuntime
	};

	// Settors
	void SetCaptureMode(const CaptureMode mode) { mCaptureMode = mode; }
	void SetTimer(const float timer) { mTimer = timer; }

	// Gettors
	CaptureMode GetCaptureMode() { return mCaptureMode; }
	float GetTimer() { return mTimer;   }

	// Output formatters
	json GetJsonResults();
	void SaveJsonResults(const std::string& path);

	// Output data gettor
	std::map<std::string, std::array<float, FRAME_COUNT>> GetData() { return mData; }


public:
	// mStartData is for start cycles of last frame only
	// mData is for resultant cycles of last FRAME_COUNT frames
	std::map<std::string, unsigned __int64> mStartData;
	std::map<std::string, std::array<float, FRAME_COUNT>> mData;

	LARGE_INTEGER mFreq;	// Used for QueryPerformanceFrequency
	LARGE_INTEGER mStartTimer, mFrameTimer;		// Time (ms)
	unsigned __int64 mFrameStart, mFrameCycles; // CPU cycles

	float mTimer = 0.0f;	// Timer for recording a single frame
	float mCurrTime = 0.0f;	// Timer to change data given an ammount of time

	// Profiler capture mode
	CaptureMode mCaptureMode = eRuntime;

	// Activation, update and capture bools
	bool mbUpdate = false;
	bool mbCapture = false;

private:
	float mFrametime = 0.0f;
};

#define Profiler ProfilerTool::Instance()