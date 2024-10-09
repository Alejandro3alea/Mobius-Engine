#pragma once

#define Singleton(class_name)				\
private:									\
	class_name() { mpInstance = nullptr; }	\
   ~class_name() { delete mpInstance; }		\
											\
	static class_name* mpInstance;			\
											\
public:										\
	static class_name* Instance()			\
	{										\
		if (!mpInstance)					\
			mpInstance = new class_name();	\
		return mpInstance;					\
	}			