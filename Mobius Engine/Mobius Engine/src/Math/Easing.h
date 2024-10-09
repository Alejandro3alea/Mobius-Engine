#pragma once

template <typename T>
T Lerp(const T & min, const T & max, float normTime)
{
	return (min + (max - min) * normTime);
}

template <typename T>
T EaseInQuad(const T & min, const T & max, float normTime)
{
	return (min + (max - min) * normTime * normTime);
}

template <typename T>
T EaseOutQuad(const T & min, const T & max, float normTime)
{
	float invTime = normTime - 1.0f;
	return (min + (max - min) * (1.0f - invTime * invTime));
}

template <typename T>
T EaseInOutQuad(const T & min, const T & max, float normTime)
{
	if (normTime < 0.5f)
		return (min + (max - min) * (2.0f * normTime * normTime));
	float invTime = normTime - 1.0f;
	return (min + (max - min) * (1.0f - 2.0f * invTime * invTime));
}