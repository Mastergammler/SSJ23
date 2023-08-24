#pragma once

#include "../IO/module.h"
#include "../imports.h"
#include "../types.h"

global_var const u32 BG_COLOR = 0x365a91;
global_var const u32 TRANS_VALUE = 0x0;

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

struct Sprite
{
    int x_tiles;
    int y_tiles;
    int sheet_start_index;

    SpriteSheet* sheet;
};

void InitBuffer(ScreenBuffer& buffer);
void DrawBitmap(ScreenBuffer& buffer,
                BitmapBuffer& bitmap,
                int bufferX,
                int bufferY,
                bool topDown = false);

/**
 * FIXME: has no clipping yet, if the screen size is not correct for this ..
 * so the input bitmap must be correct
 */
void FillWithTiles(ScreenBuffer& buffer, BitmapBuffer& bitmap);
// TODO: refactor to use sprite type
void DrawTiles(ScreenBuffer& buffer,
               int bufferStartX,
               int bufferStartY,
               SpriteSheet& sheet,
               int startTileIdx,
               int xTiles,
               int yTiles);
// difference is it assumes a 3x3 Tile template
// and is able to fill the screen if it is wider
// by reusing tiles from the 3x3 tile template
// TODO: refactor to use sprite type
void DrawPanel(ScreenBuffer& buffer,
               int bufferStartX,
               int bufferStartY,
               SpriteSheet sheet,
               int startTilePos,
               int xTiles,
               int yTiles);
void RenderFrame(HDC hdc, ScreenBuffer buffer, Dimension dim);
void ClearScreen(ScreenBuffer& buffer);

/**
 * Draws only a jagged line for now
 */
void DrawLine(ScreenBuffer& buffer,
              int startX,
              int startY,
              int targetX,
              int targetY,
              u32 color);
