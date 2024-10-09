#pragma once
#include "AL/alc.h"
#include "AL/al.h"

#include <string>
#include <fstream>
#include <iostream>
#include <bit>

struct Sound
{
    Sound(const std::string& val);
    ~Sound();

    unsigned GetBufferID() { return mBufferID; }
    unsigned GetNumChannels() { return mNumChannels; }

private:
    unsigned mBufferID;
    unsigned mNumChannels;
};