#include "Win32/module.h"
#include "globals.h"
#include "imports.h"
#include "modules.h"
#include "types.h"
#include "utils.h"

bool running = true;
Clock Time = {};
ScreenBuffer Buffer = {0, 0};

/**
 * Entry point for working with the window api
 */
int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nShowCmd)
{
    InitLogger(logger, "log.txt");

    Settings settings = LoadSettings();
    HWND window = RegisterWindow(hInstance);
    HDC hdc = GetDC(window);

    Dimension dim = GetWindowDimension(window);
    Scale = CreateWindowScale(window, settings);
    Logf("Dim: %d %d", dim.width, dim.height);

    ShowCursor(false);

    // Doesn't clip the mouse - also i have no exit button yet
    // SetCapture(window);
    InitGame(hInstance, hdc);

    Log("Starting Game Loop ...");

    timeBeginPeriod(1);

    while (running)
    {
        HandleMessages(window);
        UpdateFrame(Buffer);
        RenderFrame(hdc, Buffer, Scale.screen_dim);
        WaitTillNextFrame(window);
    }

    timeEndPeriod(1);

    return 0;
}
