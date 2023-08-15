#include "imports.h"

#ifndef types
#define types

#define global_var static
#define function static

typedef unsigned int u32;
typedef uint8_t u8;
typedef uint16_t u16;

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

struct Dimension
{
    int Width;
    int Height;
};

#endif
