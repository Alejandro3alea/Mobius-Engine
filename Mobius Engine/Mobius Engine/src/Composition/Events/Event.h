#pragma once
#include <map>
#include <string>
#include <vector>
#include <functional>

// Usage: 
// 1- Declare CREATE_EVENT/CREATE_MULTICAST_EVENT outside the scope of the class.
//			e.g.:  CREATE_MULTICAST_EVENT(OnSongUpdate, float);
// 2- Create an instance of the EventName inside the class.
//			e.g.:  OnSongUpdate mOnSongUpdate;
// 3- Add the listeners to the event.
//			e.g.:  Editor->mOnSongUpdate.Add([this](const float currMeasure) { DoSomething(currMeasure); });
// 4- Bradcast the event to execute the tasks of the listeners.
//          e.g.:  mOnSongUpdate.Broadcast(mCurrMeasure);
// 5- PROFIT!!

#define CREATE_EVENT(EventName)													\
struct EventName : public SimpleEvent											\
{																				\
	EventName() : SimpleEvent() {}												\
																				\
	virtual std::string GetName() override { return "#EventName"; }				\
}

#define CREATE_MULTICAST_EVENT(EventName,...)										\
struct EventName : public TMulticastEvent<__VA_ARGS__>								\
{																					\
	EventName() : TMulticastEvent() {}												\
																					\
	virtual std::string GetName() override { return "#EventName"; }					\
}


struct Event
{
	Event();

	virtual std::string GetName() = 0;

public:
	unsigned mId;
};



struct SimpleEvent : public Event
{
	//Este evento no es tan simple como aparenta
	//De tu gamedev de confianza
	//Guinxu
	SimpleEvent() : Event() {}

	void Add(const std::function<void()>& func) { mListeners.push_back(func); }
	void Clear() { mListeners.clear(); }
	void Broadcast()
	{
		for (auto func : mListeners)
			func();
	}

private:
	std::vector<std::function<void()>> mListeners;
};


template <typename... Ts>
struct TMulticastEvent : public Event
{
	TMulticastEvent() : Event() {}

	void Add(const std::function<void(Ts...)>& func) { mListeners.push_back(func); }
	void Clear() { mListeners.clear(); }
	void Broadcast(Ts... parameters)
	{
		for (auto func : mListeners)
			func(parameters...);
	}

private:
	std::vector<std::function<void(Ts...)>> mListeners;
};