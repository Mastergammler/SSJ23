#include "../internal.h"
#include "../systems.h"

// TODO: Refactor, this into a proper form
//  - layer drawing
//  - ui hint selection
//  - sprite input
void DrawTowerPreview(ScreenBuffer buffer,
                      SpriteSheet characterSheet,
                      SpriteSheet uiSheet,
                      Sprite bulletSprite,
                      Sprite pillarSprite)
{
    int tileIdxX = mouseState.x / Game.tile_size.width;
    int tileIdxY = mouseState.y / Game.tile_size.height;

    int tileXStart = tileIdxX * Game.tile_size.width;
    int tileYStart = tileIdxY * Game.tile_size.height;

    Tile* tile = Game.tile_map.tileAt(mouseState.x, mouseState.y);

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
        // shadow
        DrawBitmap(buffer,
                   characterSheet.tiles[shadowIdx],
                   tileXStart,
                   tileYStart);
        // ui overlay
        DrawBitmap(buffer, uiSheet.tiles[uiIdx], tileXStart, tileYStart);
        // tower pillar
        DrawTiles(buffer,
                  tileXStart,
                  tileYStart + Game.tile_size.height,
                  *pillarSprite.sheet,
                  pillarSprite.sheet_start_index,
                  pillarSprite.x_tiles,
                  pillarSprite.y_tiles);
        // tower bullet type
        int tileYOffset = pillarSprite.y_tiles * Game.tile_size.height - 4;
        DrawTiles(buffer,
                  tileXStart,
                  tileYStart + Game.tile_size.height + tileYOffset,
                  *bulletSprite.sheet,
                  bulletSprite.sheet_start_index,
                  bulletSprite.x_tiles,
                  bulletSprite.y_tiles);
    }
}

// TODO: Change pathMap to return appropriate sprite instead
// - 2nd map for id 0 or id 1 stuff
void DrawTilemap(ScreenBuffer& buffer, SpriteSheet& sheet)
{
    Tile* tile = Game.tile_map.tiles;
    for (int i = 0; i < Game.tile_map.tile_count; i++)
    {
        Tile cur = *tile++;
        int tileId = cur.tile_id;
        int tileIdx = cur.y * Game.tile_map.columns + cur.x;

        int drawX = cur.x * Game.tile_size.width;
        int drawY = (Game.tile_map.rows - 1 - cur.y) * Game.tile_size.height;

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

            sheetIdx = Game.grass_mappings[ts];
        }

        DrawBitmap(buffer, sheet.tiles[sheetIdx], drawX, drawY);

        if (cur.is_start)
        {
            DrawBitmap(buffer, Res.bitmaps.ui.tiles[3], drawX, drawY);
        }
        else if (cur.is_end)
        {
            DrawBitmap(buffer, Res.bitmaps.ui.tiles[4], drawX, drawY);
        }
    }
}

void DrawGroundLayer(ScreenBuffer buffer)
{
    DrawTilemap(buffer, Res.bitmaps.ground);
}

void DrawEntityLayer(ScreenBuffer buffer)
{
    RenderEntities(buffer);

    // TODO: should i restrict this for ui focus?
    if (ui.placement.active && !ui.ui_focus)
    {
        // FIXME: potential of nullpointer, if sprite not set yet
        DrawTowerPreview(buffer,
                         Res.bitmaps.characters,
                         Res.bitmaps.ui,
                         ui.placement.preview_bullet_sprite,
                         ui.placement.preview_pillar_sprite);
    }
}

void DrawUiLayer(ScreenBuffer buffer)
{
    RenderUiElements(buffer, Res.bitmaps.ui);

    // needs to be on top of the rest of the ui stuff
    RenderEntitiesOfType(buffer, TOWER_PROTO);
    if (ui.crafting.visible)
    {
        RenderEntitiesOfType(buffer, CRAFT_ITEM);
    }

    // draw mouse
    DrawBitmap(buffer,
               Res.bitmaps.cursor_sprite,
               mouseState.x,
               mouseState.y,
               true);
}
