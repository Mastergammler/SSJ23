#include "globals.h"
#include "imports.h"
#include "modules.h"
#include "types.h"
#include "utils.h"
#include <winuser.h>

bool running = true;
Clock Time = {};

/**
 * Entry point for working with the window api
 */
int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nShowCmd)
{
    ScreenBuffer buffer = {scale.draw_width, scale.draw_height};
    HWND window = RegisterWindow(scale, hInstance);
    HDC hdc = GetDC(window);
    // Doesn't click the mouse - also i have no exit button yet
    // SetCapture(window);
    Dimension drawableScreen = AdjustWindowScale(window);

    InitLogger(logger, "log.txt");
    InitBuffer(buffer);
    InitGame(hInstance, hdc);

    Log("Starting Game Loop ...");

    timeBeginPeriod(1);

    while (running)
    {
        HandleMessages(window);
        UpdateFrame(buffer);
        RenderFrame(hdc, buffer, drawableScreen);
        WaitTillNextFrame(window);
    }

    timeEndPeriod(1);

    return 0;
}
