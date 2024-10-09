#pragma once
#include "Singleton.h"

#include "Editor/imgui/imgui.h"

struct EditorTool
{
	virtual void Initialize() {}
	virtual void Update() {}
	virtual void Shutdown() {}

	void Enable() { mbIsEnabled = true; }
	bool IsEnabled() { return mbIsEnabled; }

protected:
	bool mbIsEnabled;
};