#pragma once

#include "imports.h"

#define global_var static
#define function static

typedef unsigned int u32;
// byte / char
typedef uint8_t u8;
// short
typedef uint16_t u16;
typedef void (*Action)();

struct Logger
{
    ofstream log_file;
    vector<string> log_buffer;
    mutex log_mutex;
    condition_variable log_condition;
    thread log_thread;

    bool running;
};

struct Dimension
{
    int Width;
    int Height;
};

struct ScreenBuffer
{
    ScreenBuffer(int width, int height)
    {
        this->width = width;
        this->height = height;
        this->size = height * width * sizeof(u32);
        this->pixel_count = height * width;

        bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmap_info.bmiHeader.biWidth = width;
        bitmap_info.bmiHeader.biHeight = height;
        bitmap_info.bmiHeader.biPlanes = 1;
        bitmap_info.bmiHeader.biBitCount = 32;
        bitmap_info.bmiHeader.biCompression = BI_RGB;
    }

    void* memory;
    int width;
    int height;
    int pixel_count;
    int size;
    BITMAPINFO bitmap_info;
};

struct WindowScale
{
  private:
    int window_height;

  public:
    int scale;
    int draw_width;
    int draw_height;
    int screen_width;
    int screen_height;

    WindowScale(int drawWidth, int drawHeight, int scale)
    {
        this->scale = scale;
        this->draw_width = drawWidth;
        this->draw_height = drawHeight;

        this->screen_width = drawWidth * scale;
        this->screen_height = drawHeight * scale;
    }

    int GetWindowHeight()
    {
        if (!window_height) return screen_height;
        return window_height;
    }

    void AdjustForTaskbarHeight(Dimension& drawableScreen)
    {
        int taskbarHeight = this->screen_height - drawableScreen.Height;
        cout << "Taskbarheight is " << taskbarHeight << endl;
        drawableScreen.Height = this->screen_height;
        this->window_height = this->screen_height + taskbarHeight;
    }
};

struct Rect
{
    int x1, x2, y1, y2;

    Rect(int x, int y, int width, int height)
    {
        this->x1 = x;
        this->y1 = y;
        this->x2 = x + width;
        this->y2 = y + height;
    }
};

// TODO: not sure if thats the way to go, but lets see
#pragma pack(push, 1)
struct Sample24
{
    unsigned int value : 24;
};
#pragma pack(pop)

struct WaveBuffer
{
    u32 sample_count;
    u16* data;
    bool loaded;
};
