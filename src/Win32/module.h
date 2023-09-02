#pragma once

#include "../IO/module.h"
#include "../Logging/module.h"
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

void HandleMessages(HWND window);
void WaitTillNextFrame(HWND window);
void ApplySettings();
HWND RegisterWindow(WindowScale scale, HINSTANCE hInstance);
Dimension AdjustWindowScale(HWND window);
