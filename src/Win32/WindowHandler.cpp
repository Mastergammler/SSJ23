#include "../globals.h"
#include "../imports.h"
#include "../types.h"
#include "FpsCounter.cpp"
#include "InputHandler.cpp"

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

function HWND RegisterWindow(int width, int height, HINSTANCE hInstance)
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
                               width,
                               height,
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
            lpmmi->ptMinTrackSize.x = scale.screen_width;
            lpmmi->ptMinTrackSize.y = scale.GetWindowHeight();
            lpmmi->ptMaxTrackSize.x = scale.screen_width;
            lpmmi->ptMaxTrackSize.y = scale.GetWindowHeight();

            cout << "Set height is: " << scale.screen_height << endl;
        }
        break;
        case WM_SIZE:
            break;
        case WM_MOUSEMOVE:
            HandleMouseMove(lParam);
            break;
        case WM_LBUTTONDOWN:
            HandleMouseButton(MOUSE_LEFT, true);
            break;
        case WM_LBUTTONUP:
            HandleMouseButton(MOUSE_LEFT, false);
            break;
        case WM_RBUTTONDOWN:
            HandleMouseButton(MOUSE_RIGHT, true);
            break;
        case WM_RBUTTONUP:
            HandleMouseButton(MOUSE_RIGHT, false);
            break;
        case WM_MBUTTONDOWN:
            HandleMouseButton(MOUSE_MIDDLE, true);
            break;
        case WM_MBUTTONUP:
            HandleMouseButton(MOUSE_MIDDLE, false);
            break;
            // TODO: handle mouse wheel? Do i need it?
            // TODO: handle focus handling?
        default:
        {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return result;
}
