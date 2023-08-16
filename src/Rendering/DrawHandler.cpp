#include "../IO/module.h"
#include "../imports.h"
#include "../types.h"

global_var u32 BG_COLOR = 0x365a91;

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

/**
 * Draws the tile from the LL (left lower) corner to the RU (right upper) corner
 * X and Y specify the start position LL for the buffer
 */
function void DrawTile(ScreenBuffer& buffer,
                       BitmapBuffer& tile,
                       int bufferX,
                       int bufferY)
{
    u32* bufferStartPos =
        (u32*)buffer.memory + bufferX + bufferY * buffer.width;
    u32* pixel = bufferStartPos;
    u32* tileValue = (u32*)tile.buffer;

    // calculate the clipping for tiles
    // else i run out of buffer memory
    int tileBoundX = bufferX + tile.width;
    int tileBoundY = bufferY + tile.height;

    int clipX = tileBoundX > buffer.width
                    ? tile.width - (tileBoundX - buffer.width)
                    : tile.width;
    int clipY = tileBoundY > buffer.height
                    ? tile.height - (tileBoundY - buffer.height)
                    : tile.height;

    for (int y = 0; y < clipY; y++)
    {
        pixel = bufferStartPos + y * buffer.width;
        for (int x = 0; x < clipX; x++)
        {
            *pixel++ = *tileValue++;
        }
    }
}
