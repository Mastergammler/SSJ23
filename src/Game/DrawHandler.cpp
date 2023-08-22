#include "internal.h"

void DrawTowerPreview(ScreenBuffer buffer,
                      SpriteSheet characterSheet,
                      SpriteSheet uiSheet)
{
    int tileIdxX = mouseState.x / tileSize.width;
    int tileIdxY = mouseState.y / tileSize.height;

    int tileXStart = tileIdxX * tileSize.width;
    int tileYStart = tileIdxY * tileSize.height;

    int tileId = tileMap.GetTileId(mouseState.x, mouseState.y);

    int uiIdx;
    int shadowIdx;
    int towerIdx;

    // tile based impact
    if (tileId == 0)
    {
        uiIdx = 0;
        shadowIdx = 17;
        towerIdx = 1;
    }
    else if (tileId == 1)
    {
        uiIdx = 2;
        shadowIdx = 16;
        towerIdx = 0;
    }

    // TODO: Layering setup etc
    // Entity layer has to be drawn, top to bottem (from tile view)
    DrawBitmap(buffer, characterSheet.tiles[shadowIdx], tileXStart, tileYStart);
    DrawBitmap(buffer, uiSheet.tiles[uiIdx], tileXStart, tileYStart);
    DrawTiles(buffer,
              tileXStart,
              tileYStart + tileSize.height / 2,
              characterSheet,
              towerIdx,
              1,
              2);
}

// TODO: the index is kind of hardcoded and not flexible
//  Change this for some kind of sheet/index mapping
void DrawTilemap(ScreenBuffer& buffer, SpriteSheet& sheet)
{
    int* tile = tileMap.idMap;
    for (int y = 0; y < tileMap.rows; y++)
    {
        for (int x = 0; x < tileMap.columns; x++)
        {
            int tileId = *tile++;
            int tileIdx = y * tileMap.columns + x;
            int tileX = x * tileSize.width;
            int tileY = y * tileSize.height;

            int sheetIdx;

            if (tileId == 0) { sheetIdx = 9; }
            else if (tileId == 1)
            {
                ContextTile tile = tileMap.context_tiles[tileIdx];
                TileEnv ts;

                // is special case - for inner corners
                if (tile.adjacent > 0b11110000) { ts = (TileEnv)tile.adjacent; }
                else
                {
                    // only compare the first 4 bits
                    ts = (TileEnv)(tile.adjacent & 0b11110000);
                }

                sheetIdx = pathMap[ts];
            }

            DrawBitmap(buffer, sheet.tiles[sheetIdx], tileX, tileY);
        }
    }
}

void DrawGroundLayer(ScreenBuffer buffer)
{
    DrawTilemap(buffer, sprites.ground);
}

void DrawEntityLayer(ScreenBuffer buffer)
{
    if (actionState.show_tower_placement && !actionState.ui_focus)
    {
        DrawTowerPreview(buffer, sprites.characters, sprites.ui);
    }
}

void DrawUiLayer(ScreenBuffer buffer)
{
    RenderUiElements(buffer, sprites.ui);

    // draw mouse
    DrawBitmap(buffer, sprites.cursor_sprite, mouseState.x, mouseState.y, true);
}
