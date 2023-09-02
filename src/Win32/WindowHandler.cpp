#include "internal.h"
#include "module.h"

function LRESULT CALLBACK WindowCallback(HWND, UINT, WPARAM, LPARAM);

WindowScale Scale = WindowScale(320, 240, 3);

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

    result.Width = clientRect.right - clientRect.left;
    result.Height = clientRect.bottom - clientRect.top;

    return result;
}

HWND RegisterWindow(WindowScale scale, HINSTANCE hInstance)
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
                               scale.screen_width,
                               scale.GetWindowHeight(),
                               0,
                               0,
                               hInstance,
                               0);
    ShowCursor(false);

    return window;
}

/**
 * Adjusting the window scale to take the task bar height into account.
 * If that is not done, the mouse coordinates will not be correct,
 * and the rendered image will be squashed (a small amount)
 */
Dimension AdjustWindowScale(HWND window)
{
    Dimension drawableDimension = GetWindowDimension(window);
    Scale.AdjustForTaskbarHeight(drawableDimension);
    SendMessage(window,
                WM_SIZE,
                SIZE_RESTORED,
                MAKELPARAM(Scale.screen_width, Scale.GetWindowHeight()));

    return drawableDimension;
}

void ApplySettings()
{
    Settings s = LoadSettings(ABSOLUTE_RES_PATH + "settings.txt");

    TargetFrameTime = 1000. / s.frame_rate;
    Scale.SetScale(s.scale);
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
            lpmmi->ptMinTrackSize.x = Scale.screen_width;
            lpmmi->ptMinTrackSize.y = Scale.GetWindowHeight();
            lpmmi->ptMaxTrackSize.x = Scale.screen_width;
            lpmmi->ptMaxTrackSize.y = Scale.GetWindowHeight();
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
