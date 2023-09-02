#pragma once

#include "../Audio/module.h"
#include "../IO/Paths.cpp"
#include "../Logging/module.h"
#include "../Rendering/module.h"
#include "../globals.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"

void InitGame(HINSTANCE hInstance, HDC hdc);
void UpdateFrame(ScreenBuffer& buffer);
