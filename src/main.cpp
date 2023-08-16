#include "globals.h"
#include "imports.h"
#include "modules.h"
#include "types.h"
#include "utils.h"

/**
 * Entry point for working with the window api
 */
int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nShowCmd)
{

    WindowScale scale = WindowScale(480, 384, 3);
    ScreenBuffer buffer = {scale.draw_width, scale.draw_height};
    HWND window =
        RegisterWindow(scale.screen_width, scale.screen_height, hInstance);
    HDC hdc = GetDC(window);

    InitLogger(logger, "log.txt");
    Log(logger, "Application started");

    InitGame(hInstance, hdc);
    InitBuffer(buffer);

    FpsCounter counter = {};
    Dimension windowDim = GetWindowDimension(window);

    Log(logger, "Starting Main Loop");

    while (running)
    {
        HandleMessages(window);
        UpdateScreen(buffer);
        RenderNextFrame(hdc, buffer, windowDim);
        UpdateTitleFps(window, &counter);
    }

    return 0;
}
