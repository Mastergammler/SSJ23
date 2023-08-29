#pragma once

#include "Win32/Timing.cpp"
#include "types.h"

global_var const float TARGET_FRAME_TIME = 1000. / 250;
global_var WindowScale scale = WindowScale(320, 240, 3);

extern bool running;
extern Logger logger;
extern Clock Time;
