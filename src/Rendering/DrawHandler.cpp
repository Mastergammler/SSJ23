#include "module.h"

function void ClearScreen(ScreenBuffer& buffer);

/**
 * Allocates the memory for the buffer and fills it with the default color
 */
void InitBuffer(ScreenBuffer& buffer)
{
    buffer.memory = VirtualAlloc(0,
                                 buffer.size,
                                 MEM_COMMIT | MEM_RESERVE,
                                 PAGE_READWRITE);
    ClearScreen(buffer);
}

void ClearScreen(ScreenBuffer& buffer)
{
    u32* pixel = (u32*)buffer.memory;
    for (int i = 0; i < buffer.pixel_count; i++)
    {
        *pixel++ = BG_COLOR;
    }
}

void DrawRect(ScreenBuffer& buffer,
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

    int clipX = tileBoundX > buffer.width ? size - (tileBoundX - buffer.width)
                                          : size;
    int clipY = tileBoundY > buffer.height ? size - (tileBoundY - buffer.height)
                                           : size;

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
void FillWithTiles(ScreenBuffer& buffer, BitmapBuffer& bitmap)
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

void DrawLine(ScreenBuffer& buffer,
              int startX,
              int startY,
              int targetX,
              int targetY,
              u32 color)
{
    assert(targetX >= startX && targetY >= startY);

    // TODO: proper bounds that don't skew the image
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (targetX >= buffer.width) targetX = buffer.width - 1;
    if (targetY >= buffer.height) targetY = buffer.height - 1;

    // TODO: bounds
    int deltaX = targetX - startX;
    int deltaY = targetY - startY;

    u32* pixel_ll = (u32*)buffer.memory;
    u32* pixel_start = pixel_ll + startY * buffer.width + startX;

    // this is how much to draw
    if (deltaX == deltaY)
    {
        for (int y = 0; y < deltaY; y++)
        {
            for (int x = 0; x < deltaX; x++)
            {
                if (x == y)
                {
                    u32* toDraw = pixel_start + y * buffer.width + x;
                    *toDraw = color;
                }
            }
        }
    }
    else if (deltaX > deltaY)
    {

        int intervallTillSwitch = deltaY > 0 ? deltaX / deltaY : deltaX;
        // for (int y = 1; y < deltaY + 1; y++)
        //{
        for (int x = 0; x < deltaX; x++)
        {
            // TODO: distribute evenly

            // skip first iteration
            if (x > 0 && x % intervallTillSwitch == 0)
            {
                // y++;
                pixel_start += buffer.width;
            }
            *pixel_start++ = color;
        }
        //}
    }
    else // deltaX < deltaY = just the inverse
    {

        int intervallTillSwitch = deltaX > 0 ? deltaY / deltaX : deltaY;
        // for (int y = 1; y < deltaY + 1; y++)
        //{
        for (int y = 0; y < deltaY; y++)
        {
            // TODO: distribute evenly

            // skip first iteration
            if (y > 0 && y % intervallTillSwitch == 0)
            {
                // x++;
                pixel_start++;
            }
            pixel_start += buffer.width;
            *pixel_start = color;
        }
        //}
    }
}

void DrawTiles(ScreenBuffer& buffer,
               int bufferStartX,
               int bufferStartY,
               SpriteSheet& sheet,
               int startTileIdx,
               int xTiles,
               int yTiles)
{
    int startY = bufferStartY + sheet.tile_height * (yTiles - 1);
    for (int y = 0; y < yTiles; y++)
    {
        int yPos = startY - (y * sheet.tile_height);
        for (int x = 0; x < xTiles; x++)
        {
            int bitmapIdx = startTileIdx + y * sheet.columns + x;
            int xPos = x * sheet.tile_width + bufferStartX;
            DrawBitmap(buffer, sheet.tiles[bitmapIdx], xPos, yPos);
        }
    }
}

void DrawPanel(ScreenBuffer& buffer,
               int bufferStartX,
               int bufferStartY,
               SpriteSheet sheet,
               int startTileIdx,
               int xTiles,
               int yTiles)
{
    auto getIndexMod = [](int pos, int posMax, int mod) {
        int factor;
        if (pos == 0)
            factor = 0;
        else if (pos < posMax)
            factor = 1;
        else
            factor = 2;

        return factor * mod;
    };

    // -1 because lower border is counting
    int startY = bufferStartY + sheet.tile_height * (yTiles - 1);

    for (int y = 0; y < yTiles; y++)
    {
        int yIndex = startTileIdx + getIndexMod(y, yTiles - 1, sheet.columns);

        int yPos = startY - (y * sheet.tile_height);
        for (int x = 0; x < xTiles; x++)
        {
            int bitmapIdx = yIndex + getIndexMod(x, xTiles - 1, 1);

            int xPos = x * sheet.tile_width + bufferStartX;
            DrawBitmap(buffer, sheet.tiles[bitmapIdx], xPos, yPos);
        }
    }
}

u32 ApplyShader(Shader shader, u32 inputColor)
{
    switch (shader.type)
    {
        case COLOR_REPLACE: return shader.shader_color;
        default: return inputColor;
    };
}

/**
 * Draws the tile from the LL (left
 * lower) corner to the RU (right upper)
 * corner X and Y specify the start
 * position LL for the buffer
 *
 * Also has the option to draw the
 * bitmap from top to bottom (means
 * starting pos in UL instead of LL /
 * does not flip the bitmap)
 */
void DrawBitmap(ScreenBuffer& buffer,
                BitmapBuffer& bitmap,
                int bufferX,
                int bufferY,
                Shader shader,
                bool topDown)
{
    if (topDown)
    {
        // -1 because the original Y should be the first pixel
        // else it would draw below it
        bufferY -= (bitmap.height - 1);
    }

    if (bufferY >= buffer.height) return;
    if (bufferX >= buffer.width) return;

    if (bufferY < 0) bufferY = 0;
    if (bufferX < 0) bufferX = 0;

    u32* bufferStart = (u32*)buffer.memory + bufferX + bufferY * buffer.width;
    u32* bitmapStart = (u32*)bitmap.buffer;

    u32* bufferPixel = bufferStart;
    u32* bitmapPixel = bitmapStart;

    // calculate the clipping for tiles
    // else i run out of buffer memory
    int tileXExc = bufferX + bitmap.width;
    int tileYExc = bufferY + bitmap.height;

    int clipXExc = tileXExc > buffer.width ? bitmap.width - (tileXExc -
                                                             buffer.width)
                                           : bitmap.width;

    int clipYExc = tileYExc > buffer.height ? bitmap.height - (tileYExc -
                                                               buffer.height)
                                            : bitmap.height;

    for (int y = 0; y < clipYExc; y++)
    {
        bufferPixel = bufferStart + y * buffer.width;
        bitmapPixel = bitmapStart + y * bitmap.width;
        for (int x = 0; x < clipXExc; x++)
        {
            if (*bitmapPixel != TRANS_VALUE)
            {
                *bufferPixel = ApplyShader(shader, *bitmapPixel);
            }
            bufferPixel++;
            bitmapPixel++;
        }
    }
}
