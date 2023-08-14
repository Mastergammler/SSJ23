#include "Win32/WindowHandler.cpp"
#include "globals.h"
#include "types.h"
#include "utils.h"
#include <memoryapi.h>
#include <windows.h>
#include <winnt.h>

function void InitBuffer(ScreenBuffer* buffer);
function void RenderBuffer(HDC hdc, ScreenBuffer buffer, Dimension* windowDim);

/**
 * Entry point for working with the window api
 */
int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nShowCmd)
{
    ScreenBuffer buffer = {1280, 720};
    HWND window = RegisterWindow(buffer, hInstance);
    HDC hdc = GetDC(window);

    Debug("Got window");

    InitBuffer(&buffer);

    FpsCounter counter = {};
    bool hasChanges = true;
    Dimension windowDim = GetWindowDimension(window);

    while (running)
    {
        HandleMessages(window);
        if (hasChanges)
        {
            // TODO: buffer
            // WriteBuffer(buffer);
            hasChanges = false;
        }
        RenderBuffer(hdc, buffer, &windowDim);
        UpdateTitleFps(window, &counter);
    }

    return 0;
}

function void InitBuffer(ScreenBuffer* buffer)
{
    buffer->memory =
        VirtualAlloc(0, buffer->size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    u32* pixel = (u32*)buffer->memory;
    for (int i = 0; i < buffer->height * buffer->width; i++)
    {
        *pixel++ = 0x365a91;
    }
}

function void RenderBuffer(HDC hdc, ScreenBuffer buffer, Dimension* windowDim)
{
    StretchDIBits(hdc,
                  0,
                  0,
                  windowDim->Width,
                  windowDim->Height,
                  0,
                  0,
                  buffer.width,
                  buffer.height,
                  buffer.memory,
                  &buffer.bitmap_info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}
