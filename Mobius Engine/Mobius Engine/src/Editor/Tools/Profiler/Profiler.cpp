#include "Profiler.h"
#include "Graphics/GfxMgr.h"
#include "Graphics/Texture.h"

#include <fstream>
#include <intrin.h>

ProfilerTool* ProfilerTool::mpInstance;

void ProfilerTool::ProfileStart(const std::string& name)
{
	// If it is not updating or active, we don't have to do any computations; return
	if (!mbIsEnabled || !mbUpdate)
		return;

	int a[4];
	int b = 0;
	// Call cpuid to prepare rdtsc
	__cpuid(a, b);
	// Call rdtsc to get cpu cycles
	mStartData[name] = __rdtsc();
}

void ProfilerTool::ProfileEnd(const std::string& name)
{
	// If it is not updating or active, we don't have to do any computations; return
	if (!mbIsEnabled || !mbUpdate)
		return;

	int a[4];
	int b = 0;
	// Call cpuid to prepare rdtsc
	__cpuid(a, b);
	// Call rdtsc to get cpu cycles
	const unsigned __int64 endTime = __rdtsc();

	// Move each one of the frame data to the right
	unsigned data_size = FRAME_COUNT - 1;
	for (unsigned i = data_size; i > 0; i--)
		mData[name][i] = mData[name][i - 1];

	// Set first value to current frame CPU cycles
	mData[name][0] = endTime - mStartData[name];
}

void ProfilerTool::FrameStart()
{
	// If it's not active, we shouldn't update data
	if (!mbIsEnabled)
		return;

	int a[4];
	int b = 0;
	// Call cpuid to prepare rdtsc
	__cpuid(a, b);
	// Call rdtsc to get cpu cycles
	mFrameStart = __rdtsc();

	QueryPerformanceCounter(&mStartTimer);
}

void ProfilerTool::FrameEnd()
{
	// If it's not active, we shouldn't update data
	if (!mbIsEnabled)
		return;

	LARGE_INTEGER FrameTimer;
	QueryPerformanceCounter(&FrameTimer);
	FrameTimer.QuadPart = FrameTimer.QuadPart - mStartTimer.QuadPart;
	float Frametime = static_cast<float>(FrameTimer.QuadPart) / mFreq.QuadPart * 1000.0f;

	// Update data
	if (mbUpdate)
	{
		int a[4];
		int b = 0;
		// Call cpuid to prepare rdtsc
		__cpuid(a, b);
		// Call rdtsc to get cpu cycles
		const unsigned __int64 EndCycles = __rdtsc();

		// Compute totaly cycles
		mFrameCycles = EndCycles - mFrameStart;

		// Set frame timer
		mFrameTimer = FrameTimer;
		mFrametime = Frametime;
	}
	mbUpdate = false;

	// Runtime loop
	if (mCaptureMode == CaptureMode::eRuntime)
	{
		mCurrTime += Frametime / 1000.0f;
		mbCapture = mCurrTime >= mTimer;
	}

	// If we want to capture the game
	if (mbCapture)
	{
		mbCapture = false;
		mbUpdate = true;
		mCurrTime = 0.0f;
	}
}

void ProfilerTool::Initialize()
{
	mbIsEnabled = true;
	mbUpdate = true;

	QueryPerformanceFrequency(&mFreq);
}

void ProfilerTool::Update()
{
	// Start "Profiler" window
	ImGui::Begin("Profiler");

	// Edit capture mode
	ImGui::Text("Capture mode:");
	// Option 1: single frame
	ImGui::RadioButton("By frame capture", reinterpret_cast<int*>(&mCaptureMode), CaptureMode::eSingleFrame); ImGui::SameLine();
	// Option 2: given time lapse
	ImGui::RadioButton("Given time lapse", reinterpret_cast<int*>(&mCaptureMode), CaptureMode::eRuntime);

	// Timer edit
	if (mCaptureMode == CaptureMode::eRuntime)
		ImGui::InputFloat("Update time", &mTimer);
	else if (ImGui::Button("Capture frame"))
		CaptureFrame();

	ImGui::Separator();
	ImGui::NewLine();

	// Print frame data
	// Print frame CPU cycles
	std::string output = "Total CPU cycles: " + std::to_string(static_cast<int>(mFrameCycles));
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), output.c_str());
	// Print frame time in milliseconds
	output = "Frametime: " + std::to_string(mFrametime) + " ms";
	ImGui::TextColored(ImVec4(1.0f, 0.25f, 1.0f, 1.0f), output.c_str());
	// Print frame FPS count
	output = "FPS: " + std::to_string(static_cast<int>(std::floor(1000.0f / mFrametime)));
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), output.c_str());
	ImGui::NewLine();
	ImGui::TextColored(ImVec4(0.25f, 1.0f, 1.0f, 1.0f), "Nodes:");

	// If no node data was detected
	if (mData.empty())
	{
		// Print usage
		ImGui::Text("No node data detected.");
		ImGui::Text("Usage: ");
		ImGui::Text("ProfileStart(x) to start node x.");
		ImGui::Text("ProfileStop(x) to stop node x.");
	}
	else
	{
		// Begin node tab bar
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			// For each node
			for (auto it : mData)
			{
				// Initialize a tab item with its name
				if (ImGui::BeginTabItem(it.first.c_str()))
				{
					// Plot graphs as helpful data
					ImGui::PlotLines("Graph", &it.second[0], it.second.size());
					// Print cycle count
					output = "CPU cycles: " + std::to_string(static_cast<int>(it.second[0]));
					ImGui::Text(output.c_str());
					// Get and print percent data
					float percent = static_cast<float>(it.second[0]) / static_cast<float>(mFrameCycles) * 100.0f;
					output = "Frame percentage: " + std::to_string(percent) + "%%";
					ImGui::Text(output.c_str());

					// End tab item
					ImGui::EndTabItem();
				}
			}

			// End node tab bar
			ImGui::EndTabBar();
		}
	}

	// End window
	ImGui::End();
}

json ProfilerTool::GetJsonResults()
{
	json outJson;

	// Get node name
	outJson[".Total CPU cycles"] = mFrameCycles;
	// Get total frametime in ms
	outJson[".Frametime (ms)"] = mFrametime;
	// Get FPS count
	int FPS = static_cast<int>(std::floor(1000.0f / mFrametime));
	outJson[".FPS"] = FPS;

	json nodesJson;
	for (auto it : mData)
	{
		json currNode;
		// Get node name
		currNode[".Node name"] = it.first;
		// Get CPU cycle count
		currNode["CPU cycles"] = it.second[0];

		// Get frame usage percentage
		float percent = static_cast<float>(it.second[0]) / static_cast<float>(mFrameCycles) * 100.0f;
		currNode["Frame percentage"] = percent;

		// push to nodes data
		nodesJson.push_back(currNode);
	}
	// Set node json
	outJson["Nodes"] = nodesJson;

	// Return resultant json data
	return outJson;
}

void ProfilerTool::SaveJsonResults(const std::string& path = "./Profiler/Frame_data.json")
{
	// If it's not active nor updating, we shouldn't save data, in order to save time
	if (!mbIsEnabled || !mbUpdate)
		return;

	// Output file stream
	std::ofstream outFile;

	// Open output file given path
	outFile.open(path);

	// Get data from 
	json outJson = GetJsonResults();

	// Set json data with proper formatting to outFile
	outFile << std::setw(4) << outJson;

	// close the output file
	outFile.close();
}