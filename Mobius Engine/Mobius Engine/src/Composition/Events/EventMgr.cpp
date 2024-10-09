#include "EventMgr.h"

EventManager* EventManager::mpInstance;

void EventManager::AddEvent(Event* eventPtr)
{
	eventPtr->mId = NewId();
	mEvents[eventPtr->mId] = eventPtr;
}

unsigned EventManager::NewId()
{
	return mNewId++;
}
