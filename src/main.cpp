#include "globals.h"
#include "imports.h"
#include "modules.h"
#include "types.h"
#include "utils.h"

function void InitBuffer(ScreenBuffer* buffer);
function void WriteToBufferTest(ScreenBuffer* buffer, BitmapBuffer* bitmap);
function void RenderBuffer(HDC hdc, ScreenBuffer buffer, Dimension* windowDim);

global_var const string ABSOLUTE_RES_PATH = "I:/02 Areas/Dev/Cpp/SSJ23/res/";

/**
 * Entry point for working with the window api
 */
int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nShowCmd)
{
    ScreenBuffer buffer = {320, 180};
    HWND window = RegisterWindow(1280, 720, hInstance);
    HDC hdc = GetDC(window);

    initLogger(logger, "log.txt");
    log(logger, "Application started");
    Debug("Started");

    vector<PlayerData> players = LoadFile("test.txt");
    AppendToFile("outputtest.txt", players);

    log(logger, "Window registered");

    BitmapBuffer bitmap =
        LoadSprite(ABSOLUTE_RES_PATH + "Test/TileTest.bmp", hInstance, hdc);
    if (bitmap.loaded) { Debug("Sprite loaded sucessfully"); }

    InitBuffer(buffer);
    DrawTiles(buffer, bitmap);

    FpsCounter counter = {};
    bool hasChanges = true;
    Dimension windowDim = GetWindowDimension(window);

    while (running)
    {
        // log(logger, "PerfTest");
        HandleMessages(window);
        if (hasChanges)
        {
            // TODO: buffer
            // WriteBuffer(buffer);
            hasChanges = false;
        }
        // TODO: rate limiting, by only rendering after a certain time period
        // that way i can modify the fps (but how to do it in a good way?)
        // i could do it kind of like vsync?
        // but this would then still impact performance, cuz of the while loop?
        // i have to restrict the while loop as well then -> i guess this is
        // complicated enough for HH? -> it seems like just limiting rendering
        // works for now
        RenderBuffer(hdc, buffer, &windowDim);
        UpdateTitleFps(window, &counter);
    }

    return 0;
}

function void RenderBuffer(HDC hdc, ScreenBuffer buffer, Dimension* windowDim)
{
    // destination is where on the window im painting
    // source is where from the buffer im taking the stuff to paint
    // NOTE: destination starts top left
    // buffer starts bottom left
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
