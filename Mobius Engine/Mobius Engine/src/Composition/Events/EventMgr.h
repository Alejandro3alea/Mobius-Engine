#pragma once
#include "Event.h"
#include "Singleton.h"

class EventManager
{
	Singleton(EventManager);

public:
	void AddEvent(Event* eventPtr);

private:
	unsigned NewId();

private:
	std::map<unsigned, Event*> mEvents;
	unsigned mNewId = 0;
};

#define EventMgr EventManager::Instance()