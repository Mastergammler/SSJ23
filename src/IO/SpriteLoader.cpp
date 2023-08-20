#include "module.h"

function HCURSOR LoadCursorIcon(HINSTANCE hInstance, string path)
{
    HCURSOR defaultCursor = LoadCursor(NULL, IDC_ARROW);

    HBITMAP hBitmap;
    hBitmap = (HBITMAP)
        LoadImage(NULL, path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    if (!hBitmap)
    {
        Log("Unable to load the file, using default cursor: " + path);
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
        Log("Failed to create cursor icon");
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
        Log("Unable to load the file: " + path);
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
