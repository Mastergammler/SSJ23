#include "internal.h"

void DrawTowerPreview(ScreenBuffer buffer,
                      SpriteSheet characterSheet,
                      SpriteSheet uiSheet,
                      Sprite towerSprite)
{
    int tileIdxX = mouseState.x / tileSize.width;
    int tileIdxY = mouseState.y / tileSize.height;

    int tileXStart = tileIdxX * tileSize.width;
    int tileYStart = tileIdxY * tileSize.height;

    int tileId = tileMap.GetTileId(mouseState.x, mouseState.y);

    int uiIdx;
    int shadowIdx;

    // tile based impact
    if (tileId == 0)
    {
        uiIdx = 0;
        shadowIdx = 17;
    }
    else if (tileId == 1)
    {
        uiIdx = 2;
        shadowIdx = 16;
    }

    // TODO: Layering setup etc
    // Entity layer has to be drawn, top to bottem (from tile view)
    if (tileId == 1)
    {
        DrawBitmap(buffer, uiSheet.tiles[uiIdx], tileXStart, tileYStart);
    }
    else
    {
        DrawBitmap(buffer,
                   characterSheet.tiles[shadowIdx],
                   tileXStart,
                   tileYStart);
        DrawBitmap(buffer, uiSheet.tiles[uiIdx], tileXStart, tileYStart);
        DrawTiles(buffer,
                  tileXStart,
                  tileYStart + tileSize.height / 2,
                  *towerSprite.sheet,
                  towerSprite.sheet_start_index,
                  towerSprite.x_tiles,
                  towerSprite.y_tiles);
    }
}

// TODO: the index is kind of hardcoded and not flexible
//  Change this for some kind of sheet/index mapping
void DrawTilemap(ScreenBuffer& buffer, SpriteSheet& sheet)
{
    Tile* tile = tileMap.tiles;
    for (int y = 0; y < tileMap.rows; y++)
    {
        for (int x = 0; x < tileMap.columns; x++)
        {

            int tileId = tile++->tile_id;
            int tileIdx = y * tileMap.columns + x;
            int tileX = x * tileSize.width;
            int tileY = y * tileSize.height;

            int sheetIdx;

            if (tileId == 0) { sheetIdx = 9; }
            else if (tileId == 1)
            {
                Tile tile = tileMap.tiles[tileIdx];
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
    DrawTilemap(buffer, bitmaps.ground);
}

void DrawEntityLayer(ScreenBuffer buffer)
{
    RenderEntities(buffer);

    if (ui.tower_placement_mode && !ui.ui_focus)
    {
        Sprite towerSprite =
            ui.tower_a_selected ? sprites.tower_a : sprites.tower_b;
        DrawTowerPreview(buffer, bitmaps.characters, bitmaps.ui, towerSprite);
    }
}

void DrawUiLayer(ScreenBuffer buffer)
{
    RenderUiElements(buffer, bitmaps.ui);

    // draw mouse
    DrawBitmap(buffer, bitmaps.cursor_sprite, mouseState.x, mouseState.y, true);
}
