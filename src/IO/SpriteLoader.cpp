#include "../globals.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"

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
            u8 c3 = 50;

            *(u32*)pixelBytes = (c3 << 24) | (c2 << 16) | (c1 << 8) | (c0 << 0);
            pixelBytes += 4;
        }
    }
};

function void PrintPixelValues(BitmapBuffer buffer)
{
    BITMAP bitmap = buffer.bitmap;
    BYTE* pixels = (BYTE*)buffer.buffer;

    for (int y = 0; y < bitmap.bmHeight; y++)
    {
        for (int x = 0; x < bitmap.bmWidth; x++)
        {
            // Get the pixel at (x, y)
            int pixelIndex = y * bitmap.bmWidth + x;
            BYTE* pixel = pixels + (pixelIndex * 4);

            // Access the pixel components (RGBA)
            BYTE red = pixel[2];
            BYTE green = pixel[1];
            BYTE blue = pixel[0];
            BYTE alpha = pixel[3];

            cout << "Pixel at " << x << "," << y << " R=" << red
                 << " G=" << green << " B=" << blue << endl;

            // Do something with the pixel values...
            // For example, print the pixel values
            printf("Pixel at (%d, %d): R=%d, G=%d, B=%d, A=%d\n",
                   x,
                   y,
                   red,
                   green,
                   blue,
                   alpha);
        }
    }
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
