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
