#include "../globals.h"
#include "FpsCounter.cpp"
#include "module.h"

function LRESULT CALLBACK WindowCallback(HWND, UINT, WPARAM, LPARAM);

/**
 * Without the default message handling the window will not respond
 * And you will not be able to close it
 */
function void HandleMessages(HWND window)
{
    MSG message;
    while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

function void UpdateTitleFps(HWND window, FpsCounter* timer)
{
    if (timer->Update())
    {
        std::stringstream ss;
        ss << timer->fps << "FPS";
        std::string title = ss.str();
        SetWindowText(window, title.c_str());
    }
}

function Dimension GetWindowDimension(HWND window)
{
    Dimension result;

    RECT clientRect;
    GetClientRect(window, &clientRect);

    result.Width = clientRect.right - clientRect.left;
    result.Height = clientRect.bottom - clientRect.top;

    return result;
}

function HWND RegisterWindow(ScreenBuffer buffer, HINSTANCE hInstance)
{
    WNDCLASS winClass = {};
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpszClassName = "GameJam Project";
    winClass.lpfnWndProc = WindowCallback;

    RegisterClass(&winClass);
    HWND window = CreateWindow(winClass.lpszClassName,
                               "SSJ 2023",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               buffer.width,
                               buffer.height,
                               0,
                               0,
                               hInstance,
                               0);
    return window;
}

LRESULT CALLBACK WindowCallback(HWND hwnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam)
{
    LRESULT result = 0;
    switch (uMsg)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            // TODO: that's not pretty -> refactor at some point
            // stop main loop
            stopLogger(logger);
            running = false;
        }
        break;
        case WM_GETMINMAXINFO:
        {
            // disable window size change
            LPMINMAXINFO lpmmi = (LPMINMAXINFO)lParam;
            lpmmi->ptMinTrackSize.x = 1280;
            lpmmi->ptMinTrackSize.y = 720;
            lpmmi->ptMaxTrackSize.x = 1280;
            lpmmi->ptMaxTrackSize.y = 720;
        }
        break;
        case WM_SIZE:
        {
            // win sound just for fun
            // somehow plays onle once
            std::cout << '\a' << std::endl;
        }
        break;
        default:
        {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return result;
}
