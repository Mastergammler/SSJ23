#pragma once

#include "../IO/module.h"
#include "../Logging/module.h"
#include "../Rendering/module.h"
#include "../globals.h"
#include "../imports.h"
#include "../types.h"

enum MouseButtons
{
    MOUSE_LEFT = 0b1,
    MOUSE_MIDDLE = 0b10,
    MOUSE_RIGHT = 0b100,
    MOUSE_X1 = 0b1000,
    MOUSE_X2 = 0b10000
};

struct Mouse
{
    int x, y;
    u8 buttons;
};

extern Mouse mouse;

HWND RegisterWindow(HINSTANCE hInstance);
Dimension GetWindowDimension(HWND window);
WindowScale CreateWindowScale(HWND& window, Settings settings);
void SetWindowSizeBasedOnTiles(WindowScale& scale,
                               int xTiles,
                               int yTiles,
                               int tileWidth,
                               int tileHeight);

void HandleMessages(HWND window);
void InitBuffer(ScreenBuffer& buffer);
void WaitTillNextFrame(HWND window);
Settings LoadSettings();
string SelectFilePath();
