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
    v2 drawPos = {tileXStart, tileYStart};

    Tile* tile = Game.tile_map.tileAt(mouseState.x, mouseState.y);

    // TODO for animation
    //  - 1. get the target pos from the keyframe
    //  - 2. move towards that keyframe
    //  -> Both pillar + bullet
    //
    //  - sprite animate the shadow according to the keyframes
    //  -> Position stays the same, just sprite changes

    if (tile->tile_id == PATH_TILE || tile->is_occupied)
    {
        DrawBitmap(buffer, uiSheet.tiles[2], drawPos.x, drawPos.y);
    }
    else
    {
        // shadow
        DrawBitmap(buffer, characterSheet.tiles[18], drawPos.x, drawPos.y);

        // ui overlay
        DrawBitmap(buffer, uiSheet.tiles[0], drawPos.x, drawPos.y);

        // tower pillar
        DrawSprite(buffer,
                   v2{drawPos.x, drawPos.y + Game.tile_size.height},
                   pillarSprite);
        //
        // tower bullet type
        int tileYOffset = pillarSprite.y_tiles * Game.tile_size.height - 4;
        DrawSprite(buffer,
                   v2{drawPos.x,
                      drawPos.y + Game.tile_size.height + tileYOffset},
                   bulletSprite);
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
        int tileIdx = cur.pos.y * Game.tile_map.columns + cur.pos.x;

        int drawX = cur.pos.x * Game.tile_size.width;
        int drawY = (Game.tile_map.rows - 1 - cur.pos.y) *
                    Game.tile_size.height;

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
    if (navigation.in_game)
    {
        RenderEntitiesOfType(buffer, TOWER_PROTO);
    }
    if (ui.crafting.visible)
    {
        RenderEntitiesOfType(buffer, CRAFT_ITEM);
    }

    // draw mouse
    DrawBitmap(buffer,
               Res.bitmaps.cursor_sprite,
               mouseState.x,
               mouseState.y,
               {},
               true);
}
