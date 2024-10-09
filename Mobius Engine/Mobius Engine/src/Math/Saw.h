#pragma once
#include <cstdlib>

// @TODO: Path functions
float Saw(const float in, const float max)
{
	int in_int = static_cast<int>(in);
	int max_int = static_cast<int>(max);
	int val = in_int - in_int % (2 * max_int);
	return max - fabs(in - (max + static_cast<float>(val)));
}