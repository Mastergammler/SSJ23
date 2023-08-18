#pragma once

#include "Logging/Logger.cpp"
#include "types.h"

global_var bool running = true;
global_var Logger logger = {};
global_var WindowScale scale = WindowScale(320, 240, 1);
global_var HCURSOR cursor;
global_var const float TARGET_FRAME_TIME = 1000. / 120;
