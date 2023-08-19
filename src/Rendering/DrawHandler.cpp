#include "../IO/module.h"
#include "../imports.h"
#include "../types.h"

global_var u32 BG_COLOR = 0x365a91;
global_var u32 TRANS_VALUE = 0x0;

function void ClearScreen(ScreenBuffer& buffer);

/**
 * Allocates the memory for the buffer and fills it with the default color
 */
function void InitBuffer(ScreenBuffer& buffer)
{
    buffer.memory =
        VirtualAlloc(0, buffer.size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    ClearScreen(buffer);
}

function void ClearScreen(ScreenBuffer& buffer)
{
    u32* pixel = (u32*)buffer.memory;
    for (int i = 0; i < buffer.pixel_count; i++)
    {
        *pixel++ = BG_COLOR;
    }
}

function void DrawRect(ScreenBuffer& buffer,
                       int targetX,
                       int targetY,
                       int size,
                       bool centered)
{
    u32* pixel = (u32*)buffer.memory;

    // calculate the clipping for tiles
    // else i run out of buffer memory

    if (centered)
    {
        targetX -= (size / 2);
        targetY -= (size / 2);

        targetX = targetX < 0 ? 0 : targetX;
        targetY = targetY < 0 ? 0 : targetY;
    }
    int tileBoundX = targetX + size;
    int tileBoundY = targetY + size;

    int clipX =
        tileBoundX > buffer.width ? size - (tileBoundX - buffer.width) : size;
    int clipY =
        tileBoundY > buffer.height ? size - (tileBoundY - buffer.height) : size;

    int pointerOffset = targetY * buffer.width + targetX;
    u32* pointerStart = pixel + pointerOffset;

    for (int y = 0; y < clipY; y++)
    {
        u32* rowStart = pointerStart + (y * buffer.width);
        for (int x = 0; x < clipX; x++)
        {
            *rowStart++ = 0xffffff;
        }
    }
}

// TODO: has no clipping and no transparency
function void DrawTiles(ScreenBuffer& buffer, BitmapBuffer& bitmap)
{
    u32* pixel = (u32*)buffer.memory;
    u32* tileStart = (u32*)bitmap.buffer;
    int maxY = buffer.height;
    int maxX = buffer.width;

    for (int y = 0; y < maxY; y++)
    {
        pixel = (u32*)buffer.memory + (y * buffer.width);
        for (int x = 0; x < maxX; x++)
        {
            int tile_x = x % bitmap.width;
            int tile_y = y % bitmap.height;
            int pixel_idx = tile_x + (tile_y * bitmap.width);

            *pixel++ = *(tileStart + pixel_idx);
        }
    }
}

// TODO: FUNCTION: draw this and next tile or something
//  - drawing multiple tiles, next to each other on some way

/**
 * Draws the tile from the LL (left lower) corner to the RU (right upper) corner
 * X and Y specify the start position LL for the buffer
 *
 * Also has the option to draw the bitmap from top to bottom
 * (means starting pos in UL instead of LL / does not flip the bitmap)
 */
function void DrawBitmap(ScreenBuffer& buffer,
                         BitmapBuffer& bitmap,
                         int bufferX,
                         int bufferY,
                         bool topDown = false)
{
    if (topDown)
    {
        // -1 because the original Y should be the first pixel
        // else it would draw below it
        bufferY -= (bitmap.height - 1);
        if (bufferY < 0) bufferY = 0;
    }

    u32* bufferStart = (u32*)buffer.memory + bufferX + bufferY * buffer.width;
    u32* bitmapStart = (u32*)bitmap.buffer;

    u32* bufferPixel = bufferStart;
    u32* bitmapPixel = bitmapStart;

    // calculate the clipping for tiles
    // else i run out of buffer memory
    // not sure if the -1 is necessary
    int tileBoundX = bufferX + bitmap.width - 1;
    int tileBoundY = bufferY + bitmap.height - 1;

    int clipXMax = tileBoundX > buffer.width
                       ? bitmap.width - (tileBoundX - buffer.width)
                       : bitmap.width;

    int clipYMax = tileBoundY > buffer.height
                       ? bitmap.height - (tileBoundY - buffer.height)
                       : bitmap.height;

    for (int y = 0; y < clipYMax; y++)
    {
        bufferPixel = bufferStart + y * buffer.width;
        bitmapPixel = bitmapStart + y * bitmap.width;
        for (int x = 0; x < clipXMax; x++)
        {
            if (*bitmapPixel != TRANS_VALUE) *bufferPixel = *bitmapPixel;
            bufferPixel++;
            bitmapPixel++;
        }
    }
}
