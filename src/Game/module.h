#pragma once

#include "../Logging/module.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"

struct TileSize
{
    int width;
    int height;
};

void InitGame(HINSTANCE hInstance, HDC hdc);
void UpdateScreen(ScreenBuffer& buffer);
