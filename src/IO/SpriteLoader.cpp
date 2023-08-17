#include "../globals.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"
#include <windef.h>
#include <wingdi.h>
#include <winuser.h>

struct BitmapBuffer
{
    BITMAP bitmap;

    /**
     * Buffer of u32 pixel values
     */
    void* buffer;
    bool loaded;

    int width;
    int height;

    BitmapBuffer() { this->loaded = false; }

    BitmapBuffer(BITMAP bitmap, void* pixels)
    {
        this->bitmap = bitmap;
        this->buffer = pixels;
        this->loaded = true;
        this->width = bitmap.bmWidth;
        this->height = bitmap.bmHeight;
    }

    BitmapBuffer(int width, int height, void* pixels)
    {
        this->width = width;
        this->height = height;
        this->loaded = true;
        this->buffer = pixels;
    }

    /**
     * Because colors come in in the wrong order, we need to switch them
     * Expected order for rendering is ARGB, incoming is BGRA
     */
    void FixChannelOrder()
    {
        if (!this->loaded) return;

        u8* pixelBytes = (u8*)this->buffer;
        for (int i = 0; i < this->width * this->height; i++)
        {
            u8 c0 = pixelBytes[0]; // blue
            u8 c1 = pixelBytes[1]; // green
            u8 c2 = pixelBytes[2]; // red

            // u8 c3 = pixelBytes[3]; // alpha
            // Alpha can not be used currently
            u8 c3 = 0;

            *(u32*)pixelBytes = (c3 << 24) | (c2 << 16) | (c1 << 8) | (c0 << 0);
            pixelBytes += 4;
        }
    }
};

function HCURSOR LoadCursorIcon(HINSTANCE hInstance, string path)
{
    HCURSOR defaultCursor = LoadCursor(NULL, IDC_ARROW);

    HBITMAP hBitmap;
    hBitmap = (HBITMAP)
        LoadImage(NULL, path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    if (!hBitmap)
    {
        Log(logger, "Unable to load the file, using default cursor: " + path);
        return defaultCursor;
    }

    BITMAP bitmap;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);

    Debug("Trying to create cursor");
    HCURSOR cursor = CreateCursor(hInstance,
                                  0,
                                  0,
                                  bitmap.bmWidth,
                                  bitmap.bmHeight,
                                  bitmap.bmBits,
                                  bitmap.bmBits);

    if (!cursor)
    {
        Log(logger, "Failed to create cursor icon");
        return defaultCursor;
    }

    return cursor;
}

/**
 * Load order of the bytes is A B G R
 */
BitmapBuffer LoadSprite(string path, HINSTANCE hInstance, HDC hdc)
{
    HBITMAP hBitmap = {};
    BITMAP bitmap = {};

    hBitmap = (HBITMAP)LoadImage(hInstance,
                                 path.c_str(),
                                 IMAGE_BITMAP,
                                 0,
                                 0,
                                 LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (!hBitmap)
    {
        Log(logger, "Unable to load the file: " + path);
        return BitmapBuffer();
    }

    GetObject(hBitmap, sizeof(BITMAP), &bitmap);
    SelectObject(hdc, hBitmap);

    BITMAPINFO bmpInfo = {};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = bitmap.bmWidth;
    bmpInfo.bmiHeader.biHeight = bitmap.bmHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    BYTE* pixels = new BYTE[bitmap.bmWidth * bitmap.bmHeight * 4];
    GetDIBits(hdc,
              hBitmap,
              0,
              bitmap.bmHeight,
              pixels,
              &bmpInfo,
              DIB_RGB_COLORS);

    cout << "Height " << dec << bitmap.bmHeight << " Width: " << bitmap.bmWidth
         << endl;

    BitmapBuffer buffer = {bitmap, pixels};
    buffer.FixChannelOrder();

    return buffer;
}

BitmapBuffer* ConvertFromSheet(BitmapBuffer sheet,
                               int tileWidth,
                               int tileHeight)
{
    u32* spriteSheet = (u32*)sheet.buffer;

    // ignore clipping, uneven counts are ignored
    int tilesPerRow = sheet.width / tileWidth;
    int tilesPerColumn = sheet.height / tileHeight;
    int tileCount = tilesPerRow * tilesPerColumn;
    int pixelsPerTile = tileWidth * tileHeight;

    BitmapBuffer* bitmaps = new BitmapBuffer[tileCount];

    for (int tileIdx = 0; tileIdx < tileCount; tileIdx++)
    {
        int tileRow = tileIdx / tilesPerRow;
        int tileCol = tileIdx % tilesPerRow;

        int tilePixelIndex =
            (sheet.width * tileHeight) * tileRow + tileCol * tileWidth;
        u32* tileStartPixel = (u32*)sheet.buffer;
        tileStartPixel += tilePixelIndex;

        u32* tilePixels = new u32[pixelsPerTile];
        u32* tilePointer = tilePixels;

        // Inverse saving because i want to specify numbers from the UL
        int mirrorIdx = MirrorIndex(tileIdx, tilesPerRow, tilesPerColumn);
        bitmaps[mirrorIdx] = BitmapBuffer{tileWidth, tileHeight, tilePointer};

        for (int y = 0; y < tileHeight; y++)
        {
            u32* pixelRow = tileStartPixel + (y * sheet.width);
            for (int x = 0; x < tileWidth; x++)
            {
                *tilePixels++ = *pixelRow++;
            }
        }
    }

    return bitmaps;
}
