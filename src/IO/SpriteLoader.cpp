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


