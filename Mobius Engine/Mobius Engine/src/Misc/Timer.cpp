#include "Timer.h"

TimeManager* TimeManager::mpInstance;

void TimeManager::StartFrame()
{
    mStartTime = std::chrono::high_resolution_clock::now();
}

void TimeManager::EndFrame()
{
    // Calculate the elapsed time since the start of the loop
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - mStartTime).count();

    // @TODO: FixedDeltaTime
    // Calculate the frame time
    deltaTime = fixedDeltaTime = elapsedTime / 1000000.0f;

    // Update the start time for the next frame
    mStartTime = endTime;
}
