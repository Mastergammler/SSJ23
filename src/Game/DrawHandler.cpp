#include "internal.h"
#include "systems.h"

// TODO: Refactor, this into a proper form
//  - layer drawing
//  - ui hint selection
//  - sprite input
void DrawTowerPreview(ScreenBuffer buffer,
                      SpriteSheet characterSheet,
                      SpriteSheet uiSheet,
                      Sprite towerSprite)
{
    int tileIdxX = mouseState.x / _tileSize.width;
    int tileIdxY = mouseState.y / _tileSize.height;

    int tileXStart = tileIdxX * _tileSize.width;
    int tileYStart = tileIdxY * _tileSize.height;

    Tile* tile = _tileMap.tileAt(mouseState.x, mouseState.y);

    int uiIdx;
    int shadowIdx;

    // tile based impact
    if (tile->tile_id == GRASS_TILE)
    {
        uiIdx = 0;
        shadowIdx = 17;
    }
    else if (tile->tile_id == PATH_TILE)
    {
        uiIdx = 2;
        shadowIdx = 16;
    }

    if (tile->tile_id == PATH_TILE || tile->is_occupied)
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
                  tileYStart + _tileSize.height / 2,
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
    Tile* tile = _tileMap.tiles;
    for (int i = 0; i < _tileMap.tile_count; i++)
    {
        Tile cur = *tile++;
        int tileId = cur.tile_id;
        int tileIdx = cur.y * _tileMap.columns + cur.x;

        int drawX = cur.x * _tileSize.width;
        int drawY = (_tileMap.rows - 1 - cur.y) * _tileSize.height;

        int sheetIdx;

        if (tileId == PATH_TILE)
        {
            sheetIdx = 33;
        }
        else if (tileId == GRASS_TILE)
        {
            TileEnv ts;

            // is special case - for inner corners
            if (cur.adjacent > 0b11110000)
            {
                ts = (TileEnv)cur.adjacent;
            }
            else
            {
                // only compare the first 4 bits
                ts = (TileEnv)(cur.adjacent & 0b11110000);
            }

            sheetIdx = grassMap[ts];
        }

        DrawBitmap(buffer, sheet.tiles[sheetIdx], drawX, drawY);

        if (cur.is_start)
        {
            DrawBitmap(buffer, _bitmaps.ui.tiles[3], drawX, drawY);
        }
        else if (cur.is_end)
        {
            DrawBitmap(buffer, _bitmaps.ui.tiles[4], drawX, drawY);
        }
    }
}

void DrawGroundLayer(ScreenBuffer buffer)
{
    DrawTilemap(buffer, _bitmaps.ground);
}

void DrawEntityLayer(ScreenBuffer buffer)
{
    RenderEntities(buffer);

    if (ui.tower_placement_mode && !ui.ui_focus)
    {
        Sprite towerSprite =
            ui.tower_a_selected ? _sprites.tower_a : _sprites.tower_b;
        DrawTowerPreview(buffer, _bitmaps.characters, _bitmaps.ui, towerSprite);
    }
}

void DrawUiLayer(ScreenBuffer buffer)
{
    RenderUiElements(buffer, _bitmaps.ui);

    // draw mouse
    DrawBitmap(buffer,
               _bitmaps.cursor_sprite,
               mouseState.x,
               mouseState.y,
               true);
}
