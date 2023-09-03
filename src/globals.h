#pragma once

#include "Win32/Timing.cpp"
#include "types.h"

global_var float TargetFrameTime = 1000. / 250;

extern bool running;
extern Logger logger;
extern Clock Time;
extern WindowScale Scale;
extern ScreenBuffer Buffer;
