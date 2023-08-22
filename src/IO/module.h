#pragma once

#include "../Logging/module.h"
#include "../globals.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"

struct BitmapBuffer
{
    bool loaded;

    int width;
    int height;

    /**
     * Buffer of u32 pixel values
     */
    void* buffer;
    BITMAP bitmap;

    BitmapBuffer() { loaded = false; }

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
        if (!loaded) return;

        u8* pixelBytes = (u8*)buffer;
        for (int i = 0; i < width * height; i++)
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

struct TileMapRaw
{
    bool loaded;

    int rows;
    int columns;

    char* data;
};

struct SpriteSheet
{
    bool loaded;

    int tile_width;
    int tile_height;
    int image_width;
    int image_height;
    int rows;
    int columns;
    int tile_count;

    BitmapBuffer* tiles;
};

TileMapRaw LoadMap(string filePath);
BitmapBuffer LoadSprite(string path, HINSTANCE hInstance, HDC hdc);
SpriteSheet ConvertFromSheet(BitmapBuffer sheet, int tileWidth, int tileHeight);
