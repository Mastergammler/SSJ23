#pragma once

#include "imports.h"

#define global_var static
#define function static

typedef unsigned int u32;
typedef uint8_t u8;
typedef uint16_t u16;

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

struct Tilemap
{
    int rows;
    int columns;
    int* idMap;

    Tilemap(const int rows, const int columns)
    {
        this->rows = rows;
        this->columns = columns;
        this->idMap = new int[rows * columns];
    }

    /**
     * Size has to match the one in the map
     * Flips the map horizontally, for rendering
     * Because rendering stars LL but for definition UL is used
     */
    void AssignMap(int* loadedMap)
    {
        int* tile = idMap;
        int* mapStart = loadedMap;

        for (int y = rows - 1; y >= 0; y--)
        {
            int* rowStart = mapStart + y * columns;
            for (int x = 0; x < columns; x++)
            {
                *tile++ = *rowStart++;
            }
        }
    }
};
