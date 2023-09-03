#include "internal.h"
#include "module.h"
#include <winnt.h>

function LRESULT CALLBACK WindowCallback(HWND, UINT, WPARAM, LPARAM);

WindowScale Scale = {800, 600};
HWND registeredWindow;

/**
 * Without the default message handling the window will not respond
 * And you will not be able to close it
 */
void HandleMessages(HWND window)
{
    MSG message;
    while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

void WaitTillNextFrame(HWND window)
{
    Time.Update();

    float frameTimeMs = Time.delta_time_real * 1000;
    float maxTimeMs = Time.delta_max * 1000;
    float minTimeMs = Time.delta_min * 1000;

    if (Time.ShowValue())
    {
        std::stringstream ss;
        ss << setprecision(3) << Time.fps << " FPS - " << frameTimeMs
           << " ms/f";

        // NOTE: this dosn't work correctly, because if you touch the window
        //  then the min gets to 0 so it is not repsective of the actual value
        //<< " | (" << Time.fps_min << " | " << timer.fps_max << " ) Fps - ("
        //<< minTimeMs << " | " << maxTimeMs << ") ms/f";
        std::string title = ss.str();
        SetWindowText(window, title.c_str());
    }

    if (frameTimeMs < TargetFrameTime)
    {
        DWORD timeToRest = (DWORD)(TargetFrameTime - frameTimeMs);
        if (timeToRest > 0)
        {
            Sleep(timeToRest);
        }
    }

    // further waiting if still not there
    // this seems to lead to alot more consistent frame times
    while (Time.CheckDeltaTimeMs() < TargetFrameTime)
    {
        Sleep(0);
    }
}

Dimension GetWindowDimension(HWND window)
{
    Dimension result;

    RECT clientRect;
    GetClientRect(window, &clientRect);
    result.width = clientRect.right - clientRect.left;
    result.height = clientRect.bottom - clientRect.top;

    return result;
}

HWND RegisterWindow(HINSTANCE hInstance)
{
    WNDCLASS winClass = {};
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpszClassName = "DefenceIY";
    winClass.lpfnWndProc = WindowCallback;

    RegisterClass(&winClass);
    HWND window = CreateWindow(winClass.lpszClassName,
                               "A window",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               // NOTE: this doesn't matter, because this is
                               // just the value that invokes the change window
                               // size event, and i have overritten this to use
                               // the scaler size
                               0,
                               0,
                               0,
                               0,
                               hInstance,
                               0);
    // TODO: how to handle this better?
    registeredWindow = window;

    return window;
}

/**
 * creates the initial scale and calculates task bar height
 */
WindowScale CreateWindowScale(HWND& window, Settings settings)
{
    Dimension drawableDimension = GetWindowDimension(window);
    assert(drawableDimension.width = Scale.screen_dim.width);
    int taskbarHeight = Scale.window_height - drawableDimension.height;
    Logf("Taskbar height %d", taskbarHeight);

    WindowScale scale = {};

    scale.taskbar_height = taskbarHeight;
    scale.scale = settings.scale;

    return scale;
}

/**
 * Allocates the memory for the buffer and fills it with the default color
 */
void InitBuffer(ScreenBuffer& buffer)
{
    buffer.memory = VirtualAlloc(0,
                                 buffer.size,
                                 MEM_COMMIT | MEM_RESERVE,
                                 PAGE_READWRITE);
    ClearScreen(buffer);
}

void SetSizeBasedOnTiles(WindowScale& scale,
                         int xTiles,
                         int yTiles,
                         int tileWidth,
                         int tileHeight)
{
    scale.render_dim.width = xTiles * tileWidth;
    scale.render_dim.height = yTiles * tileHeight;
    scale.screen_dim.width = scale.render_dim.width * scale.scale;
    scale.screen_dim.height = scale.render_dim.height * scale.scale;
    scale.window_height = scale.screen_dim.height + scale.taskbar_height;

    VirtualFree(Buffer.memory, 0, MEM_RELEASE);
    Buffer = {scale.render_dim.width, scale.render_dim.height};
    InitBuffer(Buffer);

    SetWindowPos(registeredWindow,
                 NULL,
                 0,
                 0,
                 Scale.screen_dim.width,
                 Scale.screen_dim.height,
                 SWP_NOMOVE | SWP_NOZORDER);
}

Settings LoadSettings()
{
    Settings settings = LoadSettings(ABSOLUTE_RES_PATH + "settings.txt");
    // TODO: setting targets here is pretty ugly
    TargetFrameTime = 1000. / settings.frame_rate;

    return settings;
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
            // stop main loop -> want some general function probably
            stopLogger(logger);
            running = false;
        }
        break;
        // NOTE: this event is called every time i touch the window
        case WM_GETMINMAXINFO:
        {
            // Disallow window change from user
            // -> we always use our scalings
            LPMINMAXINFO lpmmi = (LPMINMAXINFO)lParam;
            lpmmi->ptMinTrackSize.x = Scale.screen_dim.width;
            // NOTE: need to use the actual window size -> means drawable screen
            // + taskbar height = window-height
            lpmmi->ptMinTrackSize.y = Scale.window_height;
            lpmmi->ptMaxTrackSize.x = Scale.screen_dim.width;
            lpmmi->ptMaxTrackSize.y = Scale.window_height;
        }
        break;
        case WM_SETFOCUS:
        {
            HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
            SetCursor(hCursor);
        }
        break;
        case WM_SIZE: break;
        case WM_MOUSEMOVE: HandleMouseMove(lParam); break;
        case WM_LBUTTONDOWN: HandleMouseButton(MOUSE_LEFT, true); break;
        case WM_LBUTTONUP: HandleMouseButton(MOUSE_LEFT, false); break;
        case WM_RBUTTONDOWN: HandleMouseButton(MOUSE_RIGHT, true); break;
        case WM_RBUTTONUP: HandleMouseButton(MOUSE_RIGHT, false); break;
        case WM_MBUTTONDOWN: HandleMouseButton(MOUSE_MIDDLE, true); break;
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
