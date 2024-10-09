#include "Event.h"
#include "EventMgr.h"

Event::Event()
{
	EventMgr->AddEvent(this);
}
