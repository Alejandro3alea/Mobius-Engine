#pragma once
#include "Misc/ColorConsole.h"

#undef Ensure
#define Ensure(condition, message)	\
	bool _conditionVal = condition;	\
	if (_conditionVal == false)		\
	{								\
		PrintError(message);		\
		abort();					\
	}