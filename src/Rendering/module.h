#pragma once

#include "../IO/module.h"
#include "../imports.h"
#include "../types.h"

global_var const u32 BG_COLOR = 0x365a91;
global_var const u32 TRANS_VALUE = 0x0;

void InitBuffer(ScreenBuffer& buffer);
void DrawBitmap(ScreenBuffer& buffer,
                BitmapBuffer& bitmap,
                int bufferX,
                int bufferY,
                bool topDown = false);
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
