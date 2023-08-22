#include "internal.h"

// TODO: Refactor, this into a proper form
//  - layer drawing
//  - ui hint selection
//  - sprite input
void DrawTowerPreview(ScreenBuffer buffer,
                      SpriteSheet characterSheet,
                      SpriteSheet uiSheet,
                      Sprite towerSprite)
{
    int tileIdxX = mouseState.x / tileSize.width;
    int tileIdxY = mouseState.y / tileSize.height;

    int tileXStart = tileIdxX * tileSize.width;
    int tileYStart = tileIdxY * tileSize.height;

    Tile* tile = tileMap.tileAt(mouseState.x, mouseState.y);

    int uiIdx;
    int shadowIdx;

    // tile based impact
    if (tile->tile_id == 0)
    {
        uiIdx = 0;
        shadowIdx = 17;
    }
    else if (tile->tile_id == 1)
    {
        uiIdx = 2;
        shadowIdx = 16;
    }

    if (tile->tile_id == 1 || tile->is_occupied)
    {
        DrawBitmap(buffer, uiSheet.tiles[2], tileXStart, tileYStart);
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

// TODO: Change pathMap to return appropriate sprite instead
// - 2nd map for id 0 or id 1 stuff
void DrawTilemap(ScreenBuffer& buffer, SpriteSheet& sheet)
{
    Tile* tile = tileMap.tiles;
    for (int i = 0; i < tileMap.tile_count; i++)
    {
        Tile cur = *tile++;
        int tileId = cur.tile_id;
        int tileIdx = cur.y * tileMap.columns + cur.x;

        int drawX = cur.x * tileSize.width;
        int drawY = (tileMap.rows - 1 - cur.y) * tileSize.height;

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

        DrawBitmap(buffer, sheet.tiles[sheetIdx], drawX, drawY);
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
