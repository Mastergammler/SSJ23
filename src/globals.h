#pragma once

#include "Win32/FpsCounter.cpp"
#include "types.h"

global_var bool running = true;
global_var Logger logger = {};
global_var WindowScale scale = WindowScale(320, 240, 3);
global_var HCURSOR cursor;
global_var const float TARGET_FRAME_TIME = 1000. / 120;
global_var FpsCounter counter = {};
